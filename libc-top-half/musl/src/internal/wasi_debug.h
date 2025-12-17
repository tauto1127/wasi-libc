#ifndef _WASI_DEBUG_H
#define _WASI_DEBUG_H

#include <stdio.h>

#ifdef WASI_LIBC_DEBUG
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...) (void)0
#endif

#endif
