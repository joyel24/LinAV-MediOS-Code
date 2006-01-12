/* 
*   include/errors.h
*
*   AMOS project
*
*/

#ifndef __ERRORS_H
#define __ERRORS_H

typedef int MED_RET_T;

#define MED_OK               0 /* everything is ok */
#define MED_ENOENT           1 /* missing file /dir */
#define MED_EIO              2 /* IO error */
#define MED_ERROR            3 /* something is going wrong */
#define MED_ENOMEM           4 /* No more available memory */

#endif
