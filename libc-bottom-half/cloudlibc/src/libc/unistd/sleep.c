// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// SPDX-License-Identifier: BSD-2-Clause

#include <stdio.h>
#include <time.h>
#include <unistd.h>

unsigned int sleep(unsigned int seconds) {
  struct timespec ts = {.tv_sec = seconds, .tv_nsec = 0};
  printf("sleep called: %u\n", seconds);
  if (clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL) != 0)
    return seconds;
  return 0;
}
