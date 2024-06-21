#ifndef _UTIL_H
#define _UTIL_H

#define TAKE /* transfer ownership (should free later) */
#define SHARE /* share ownership (no need to free yourself) */

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif
