#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */


DLLIMPORT void HelloWorld (void);

/*-----------------------------------------------------------------------------
    file:   boyer.h
    desc:   Boyer-Moore text search algorithm (Windows version) header
    by:     Patrick Ko
    date:   6 Mar 91 - born
    revi:   4 Apr 94 - port Windows 3.1
            21 Aug 94 - support Windows DLL
    note:   use huge pointers to cater for big contiguous memory
-----------------------------------------------------------------------------*/

#ifndef HPSTR
typedef BYTE _huge *    HPSTR;
#endif

/* store the pattern, pattern length and skip table for 256 alphabets */
/* maximum pattern length (MAXPAT) cannot be larger than 65535 */

#define MAXPAT  256 + 1

/* boyer.c prototypes */

void FAR PASCAL SetFindPattern( LPSTR lpszPattern );
LPSTR FAR PASCAL Find( LPSTR s, LONG slen );
LPSTR FAR PASCAL FindBackward( LPSTR s, LONG slen );
LPSTR FAR PASCAL FindIC( LPSTR s, LONG slen );
LPSTR FAR PASCAL FindBackwardIC( LPSTR s, LONG slen );



#endif /* _DLL_H_ */
