#include "dll.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

DLLIMPORT void HelloWorld ()
{
    MessageBox (0, "Hello World from DLL!\n", "Hi", MB_ICONINFORMATION);
}


BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

/*-----------------------------------------------------------------------------
    file:   boyer.c
    desc:   Boyer-Moore text search algorithm (Windows version)
    by:     Patrick Ko
    date:    6 Mar 91 - born
    revi:    4 Apr 94 - port Windows 3.1
            21 Aug 94 - support Windows DLL
    note:   use huge pointers to cater for big contiguous memory
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
    Program Specification

    in:     search space s, pattern p
    out:    a pointer where p is exactly matched at s[i], NULL indicates fail
    why:    Boyer-Moore algorithm is best for general text search. On
            "average" it takes length(s)/length(p) steps to match p in s.

    ref:    I recommend the following references:

            "Algorithms". Robert Sedgewick. Addison Wesley Publishing Company.
            1988. 2nd addition. p286. QA76.6.S435 1983

            "Faster String Searches". Doctor Dobb's Journal. Volume 14
            Issue 7 July 1989. Costas Menico. p74.

    usage:  e.g. to find a pattern "tiger" in a text in RAM starting at
                 pointer "txtp" with a length of 1,000,000 characters,
                 program like this:

            LPSTR matchp;

            SetFindPattern( "tiger" );
            matchp = Find( txtp, 1000000L );
            if (matchp != NULL)
                // found
            else
                // not found

            matchp = FindBackward( txtp + 1000000L - 1, 1000000L);
            if (matchp != NULL)
                // found
            else
                // not found


    Q:      Can I use Find() with a GlobalLock() pointer in Windows?
    A:      Yes.

    Q:      Must I delcare my pointer as HPSTR (huge pointer) ?
    A:      Not necessary.  Find() and FindBackward() will convert your
            LPSTR as HPSTR.  However, in your own code you must aware
            that you are holding a LPSTR and take care of the pointer
            arithmetic and conversion. (see demo.c for example)

    Q:      What is the limit of the memory space I can search?
    A:      To the limit of huge pointer implementation and your hardware.

-----------------------------------------------------------------------------*/

#include <windows.h>
#include <ctype.h>
#include "boyer.h"

static BYTE p[MAXPAT];
static int  plen;
static int  skip[256];
static HANDLE hInstanceDLL = NULL;

UINT WINAPI WEP(int nParam)
{
    nParam;
	return 1;
}

int FAR PASCAL LibMain(
    HANDLE hInstance, UINT wDataSeg, UINT cbHeapSize, LPSTR lpszCmdLine)
{
	wDataSeg;
	lpszCmdLine;

	if (!cbHeapSize)
        return 0;

	UnlockData(0);
    return 1;
}

/*-----------------------------------------------------------------------------
    func:   SetFindPattern
    desc:   initialize the pattern to be matched and generate skip table
    pass:   pat = pattern string
    rtrn:   nothing
-----------------------------------------------------------------------------*/
void FAR PASCAL SetFindPattern( LPSTR lpszPattern )
{
	int j;

    if ((plen = lstrlen( lpszPattern )) <= MAXPAT)
    {
        lstrcpy( p, lpszPattern );

        for (j=0; j<256; j++)
        {
            *(skip + j) = plen;
        }

        for (j=1; j<=plen; j++)
        {
            *(skip + *(lpszPattern + j - 1)) = plen - j;
        }
    }
}

/*-----------------------------------------------------------------------------
    func:   Find
    desc:   match a pattern defined in SetFindPattern against string s
    pass:   s = start of search space, slen = length of s
    rtrn:   NULL = match fail
            else = a LPSTR to p[0] in s matches p
-----------------------------------------------------------------------------*/
LPSTR FAR PASCAL Find( LPSTR s, LONG slen )
{
    LONG i, j;

    i = j = (LONG)plen;

	do
    {
        if (*((HPSTR)s + i - 1) == p[j - 1])
        {
			i--;
			j--;
        }
		else
        {
            if (plen - j + 1 > *(skip + *((HPSTR)s + i - 1)) )
            {
                i += plen - j + 1;
            }
			else
            {
                i += *(skip + *((HPSTR)s + i - 1));
            }
			j = plen;
        }
    }
    while (j >= 1 && i <= slen);

    /* match fails */
    if (i >= slen)
        return (NULL);

    /* match successful */
    else
        return ((LPSTR)((HPSTR)s + i));
}

/*-----------------------------------------------------------------------------
    func:   FindBackward
    desc:   match a pattern defined in SetFindPattern against string s
            in backward manner
    pass:   s = start of search space, slen = length of s
    rtrn:   NULL = match fail
            else = a LPSTR to p[0] in s matches p
-----------------------------------------------------------------------------*/
LPSTR FAR PASCAL FindBackward( LPSTR s, LONG slen )
{
    LONG i, j;

    i = j = (LONG)plen;

	do
    {
        if (*((HPSTR)s + 1 - i ) == p[plen - j])
        {
			i--;
			j--;
        }
		else
        {
            if (plen - j + 1 > *(skip + *((HPSTR)s + 1  - i)) )
            {
                i += plen - j + 1;
            }
            else
            {
                i += *(skip + *((HPSTR)s + 1  - i));
            }
			j = plen;
        }
    }
    while (j >= 1 && i <= slen);

    /* match fails */
    if (i >= slen)
        return (NULL);
    /* match successful */
	else
        return ((LPSTR)((HPSTR)s - i - plen + 1));
}

/*-----------------------------------------------------------------------------
    func:   FindIC
    desc:   match a pattern defined in SetFindPattern against string s
            and Ignore Case (i.e. case insensitive)
    pass:   s = start of search space, slen = length of s
    rtrn:   NULL = match fail
            else = a LPSTR to p[0] in s matches p
-----------------------------------------------------------------------------*/
LPSTR FAR PASCAL FindIC( LPSTR s, LONG slen )
{
    LONG i, j;

    i = j = (LONG)plen;

	do
    {
        if (toupper(*((HPSTR)s + i - 1)) == toupper(p[j - 1]))
        {
			i--;
			j--;
        }
		else
        {
            if (plen - j + 1 > *(skip + *((HPSTR)s + i - 1)) )
            {
                i += plen - j + 1;
            }
			else
            {
                i += *(skip + *((HPSTR)s + i - 1));
            }
			j = plen;
        }
    }
    while (j >= 1 && i <= slen);

    /* match fails */
    if (i >= slen)
        return (NULL);

    /* match successful */
    else
        return ((LPSTR)((HPSTR)s + i));
}

/*-----------------------------------------------------------------------------
    func:   FindBackwardIC
    desc:   match a pattern defined in SetFindPattern against string s
            in backward manner and ignore case (i.e. case insensitive)
    pass:   s = start of search space, slen = length of s
    rtrn:   NULL = match fail
            else = a LPSTR to p[0] in s matches p
-----------------------------------------------------------------------------*/
LPSTR FAR PASCAL FindBackwardIC( LPSTR s, LONG slen )
{
    LONG i, j;

    i = j = (LONG)plen;

	do
    {
        if (toupper(*((HPSTR)s + 1 - i )) == toupper(p[plen - j]))
        {
			i--;
			j--;
        }
		else
        {
            if (plen - j + 1 > *(skip + *((HPSTR)s + 1  - i)) )
            {
                i += plen - j + 1;
            }
            else
            {
                i += *(skip + *((HPSTR)s + 1  - i));
            }
			j = plen;
        }
    }
    while (j >= 1 && i <= slen);

    /* match fails */
    if (i >= slen)
        return (NULL);
    /* match successful */
	else
        return ((LPSTR)((HPSTR)s - i - plen + 1));
}
