#include "fr_utils.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>

#include <fcntl.h>
#include <sched.h>

#include <immintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>


void *map_offset(const char *file, uint64_t offset) {

  int fd = open(file, O_RDONLY);
  if (fd < 0)
    return NULL;
  
  char *mapaddress = MAP_FAILED;
  #ifdef HAVE_MMAP64
    mapaddress = mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_PRIVATE, fd, offset & ~(sysconf(_SC_PAGE_SIZE) -1));
  #else
    mapaddress = mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_PRIVATE, fd, ((off_t)offset) & ~(sysconf(_SC_PAGE_SIZE) -1));
  #endif
  close(fd);
  if (mapaddress == MAP_FAILED)
    return NULL;
  return (void *)(mapaddress+(offset & (sysconf(_SC_PAGE_SIZE) -1)));
}

inline int memaccess(void *v) {
  int rv;
  asm volatile("mov (%1), %0": "+r" (rv): "r" (v):);
  return rv;
}

void delayloop(uint32_t cycles) {
  uint64_t start = rdtscp64();
  while ((rdtscp64()-start) < cycles)
    ;
}

inline uint32_t memaccesstime(void *v) {
  uint32_t rv;
  asm volatile (
      "mfence\n"
      "lfence\n"
      "rdtscp\n"
      "mov %%eax, %%esi\n"
      "mov (%1), %%eax\n"
      "rdtscp\n"
      "sub %%esi, %%eax\n"
      : "=&a" (rv): "r" (v): "ecx", "edx", "esi");
  return rv;
}

inline void clflush(void *v) {
  asm volatile ("clflush 0(%0)": : "r" (v):);
}

inline uint32_t rdtscp() {
  uint32_t rv;
  asm volatile ("rdtscp": "=a" (rv) :: "edx", "ecx");
  return rv;
}

inline uint64_t rdtscp64() {
  uint32_t low, high;
  asm volatile ("rdtscp": "=a" (low), "=d" (high) :: "ecx");
  return (((uint64_t)high) << 32) | low;
}

inline void mfence() {
  asm volatile("mfence");
}

int64_t reload(void *addr) {
  int64_t start = _rdtsc();
  memaccess(addr);
  return _rdtsc() - start;
}

int64_t flush_and_reload(void *addr) {
  int64_t delta = memaccesstime(addr);
  clflush(addr);
  return delta > UINT16_MAX ? UINT16_MAX : delta;
}
