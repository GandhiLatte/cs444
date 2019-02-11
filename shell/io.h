#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define fatal(...) fprintf(stderr, __VA_ARGS__), abort()

/* The macro above can be thought to provide this function:

void fatal(char *s, ...);
*/

#endif
