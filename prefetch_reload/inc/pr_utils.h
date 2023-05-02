#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <immintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "pthread.h"
#include <semaphore.h>
#include <sched.h>
#include <fcntl.h>

#define TRY 10000

void *map_offset(const char *, uint64_t);

void delayloop(uint32_t);

void prefetchw(void *v);

int memaccess (void *v);

uint32_t memaccesstime (void *v);

void prefetch (void *v);

void mfence ();

void clflush (void *v);

uint32_t rdtscp ();

uint64_t rdtscp64 ();

void clflush(void *v);

int64_t reload (void *addr);

int64_t pr_calibration (bool);
