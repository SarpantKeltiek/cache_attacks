#include <stdint.h>
#include <stdlib.h>

void *map_offset(const char *, uint64_t);

void delayloop(uint32_t);

int memaccess(void *v);

uint32_t memaccesstime(void *v);

void mfence();

void clflush(void *v);

uint32_t rdtscp();

uint64_t rdtscp64();

int64_t reload(void *addr);

int64_t flush_and_reload(void *addr);