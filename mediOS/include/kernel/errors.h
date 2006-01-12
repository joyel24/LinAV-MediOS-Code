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
#define MED_ENOENT           1 /* missing data (file/dir/other) */
#define MED_EIO              2 /* IO error */
#define MED_ERROR            3 /* something is going wrong */
#define MED_ENOMEM           4 /* No more available memory */
#define MED_EEXIST           5 /* already exists */
#define MED_EBUSY            6 /* Object still in use */
#define MED_EINVAL           7 /* bad arg error */
#define MED_EBADDATA         8 /* bad data */
#endif
