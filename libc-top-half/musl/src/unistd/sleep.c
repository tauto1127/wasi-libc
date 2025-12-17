#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "wasi_debug.h"

unsigned sleep(unsigned seconds)
{
    DEBUG_PRINTF("sleep wasi-libc: %u\n" ,seconds);
	struct timespec tv = { .tv_sec = seconds, .tv_nsec = 0 };
	if (nanosleep(&tv, &tv))
		return tv.tv_sec;
	return 0;
}
