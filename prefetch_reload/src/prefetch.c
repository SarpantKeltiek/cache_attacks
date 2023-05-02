
/*
@author: Sarpant
*/

#define _GNU_SOURCE

#include <err.h>

#include "pr_utils.h"
#include "test_lib.h"




static sem_t sem_trojan;
static sem_t sem_spy;

static size_t threshold = 0;

/* trojan thread. Prefetch line in cache, and wait for spy */
static void * trojan (void * addr) {

  memaccesstime(addr);

  for (size_t i = 0; i < TRY; i++) {

    sem_wait(&sem_spy);

    asm volatile("lfence");
    prefetchw(addr);

    sem_post(&sem_trojan);
  }
}

/* Spy trhead. Wait for trojan, and measure timeacces to data */
static void * spy (void * addr) {

  size_t delta;
  for (size_t i = 0; i < TRY; i++) {

    sem_wait(&sem_trojan);

    delayloop(1000000000);
    asm volatile("lfence");
    delta = memaccesstime(addr);
    delta = delta > UINT16_MAX ? UINT16_MAX : delta;
    if (delta < threshold)
      printf("Delta: %ld - Access !\n", delta);

    sem_post(&sem_spy);
  }

}

int main () {

  puts("[+] Starting calibration ...");

  /*size_t threshold_noaccess = pr_calibration(true);
  size_t threshold_access = pr_calibration(false);
  size_t threshold = (threshold_access + threshold_noaccess)/2;

  printf("Calibration done ! Threshold = %ld & %ld -> %ld\n ", threshold_noaccess, threshold_access, threshold);
*/

  void *addr = map_offset("test/secret.txt", 0);

  threshold = 45;

  if (addr == NULL)
    errx(EXIT_FAILURE, "Error while mapping file");

  sem_init(&sem_trojan, 0, 0);
  sem_init(&sem_spy, 0, 1);

  pthread_t trojan_thread, spy_thread;
  pthread_attr_t attr_trojan;
  pthread_attr_t attr_spy;

  pthread_attr_init(&attr_trojan);
  pthread_attr_init(&attr_spy);

  cpu_set_t cpuset_trojan;
  CPU_ZERO(&cpuset_trojan);
  CPU_SET(0, &cpuset_trojan);
  if (pthread_attr_setaffinity_np(&attr_trojan, sizeof(cpu_set_t), 
                                  &cpuset_trojan) != 0) {
    perror("pthread_attr_setaffinity_np");
    exit(EXIT_FAILURE);
  }

  cpu_set_t cpuset_spy;
  CPU_ZERO(&cpuset_spy);
  CPU_SET(1, &cpuset_spy);
  if (pthread_attr_setaffinity_np(&attr_spy, sizeof(cpu_set_t), 
                                  &cpuset_spy) != 0) {
    perror("pthread_attr_setaffinity_np");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&trojan_thread, &attr_trojan, (void *) trojan, addr) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&spy_thread, &attr_spy, (void *) spy, addr) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  pthread_join(trojan_thread, NULL);
  pthread_join(spy_thread, NULL);

  sem_destroy(&sem_trojan);
  sem_destroy(&sem_spy);

  return 0;
}
