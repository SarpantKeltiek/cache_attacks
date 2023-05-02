
#include "pr_utils.h"

/* Mastik file mapping function with offset */
void *map_offset (const char *file, uint64_t offset) {

  int fd = open(file, O_RDONLY);
  if (fd < 0)
    return NULL;

  char *mapaddress = MAP_FAILED;

  #ifdef HAVE_MMAP64
    mapaddress = mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_PRIVATE, fd, offset & ~(sysconf(_SC_PAGE_SIZE) - 1));
  #else
    mapaddress = mmap(0, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_PRIVATE, fd, ((off_t)offset) & ~(sysconf(_SC_PAGE_SIZE) - 1));
  #endif
  close(fd);

  if (mapaddress == MAP_FAILED)
    return NULL;

  return (void *)(mapaddress + (offset & (sysconf(_SC_PAGE_SIZE) - 1)));
}

/* Mastik function to wait cycles cycles*/
void delayloop(uint32_t cycles)
{
  uint64_t start = rdtscp64();
  while ((rdtscp64() - start) < cycles)
    ;
}

inline void prefetchw(void *v) {
  asm volatile("prefetchw (%0)" :: "r" (v));
}

inline int memaccess(void *v) {
  int rv;
  asm volatile("mov (%1), %0": "+r" (rv): "r" (v):);
  return rv;
}

/* function in line to measure timeacces to data */
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

/* function in line to prefetch line in cache memory*/
inline void prefetch(void *v) {
  asm volatile("prefetcht0 (%0)" :: "r" (v));
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

inline void clflush(void *v)
{
  asm volatile("clflush 0(%0)"
               :
               : "r"(v)
               :);
}

inline void mfence() {
  asm volatile("mfence");
}

int64_t reload(void *addr) {
  int64_t start = _rdtsc();
  memaccess(addr);
  return _rdtsc() - start;
}

void action (int signum) {}

/* trojan thread. Prefetch line in cache, and wait for spy */
void * c_trojan (void * line, size_t try, pid_t spy, sigset_t * sigset, bool phase) {

  for (size_t i = 0; i < try; i++) {
    memaccesstime(line);
    if (phase) {
      asm volatile("mfence");
      asm volatile("lfence");
      __builtin_prefetch(line, 1, 3);
    }
    kill(spy, SIGUSR1);
    sigsuspend(sigset);
  }
}

/* Spy trhead. Wait for trojan, and measure timeacces to data */
int64_t c_spy (void * line, size_t try, pid_t trojan, sigset_t * sigset, bool phase) {

  size_t res = 0;
  for (size_t i = 0; i < try; i++) {

    sigsuspend(sigset);
    /* wait for trojan */
    usleep(1000);
    if (!phase) {
      asm volatile("mfence");
      asm volatile("lfence");
      memaccesstime(line);
    }
    res += memaccesstime(line);
    kill(trojan, SIGUSR1);
  }

  res = res/try;
  return res;
}

int64_t pr_calibration (bool phase) {

  pid_t pid_trojan = getpid();
  pid_t pid = fork();

  sigset_t sigset;
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGUSR1);
  sigprocmask(SIG_BLOCK, &sigset, NULL);
  sigemptyset(&sigset);
  signal(SIGUSR1, action);

  size_t try = 10000;
  int status;

  static char dummy;

  size_t res = 0;

  if (pid) {

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask); /* we chosse ce cpu 0 for trojan */

    sched_setaffinity(0, sizeof(cpu_set_t), &mask); /* set the cpu mask */

    res = c_spy(&dummy, try, pid, &sigset, phase);

    waitpid(pid, &status, WUNTRACED);
  }

  else {

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask); /* we chosse ce cpu 1 for spy */

    sched_setaffinity(0, sizeof(cpu_set_t), &mask); /* set the cpu mask */

    c_trojan(&dummy, try, getppid(), &sigset, phase);

    exit(EXIT_SUCCESS);
  }

  return res;
}

