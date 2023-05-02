/*
  Author = Sarpant & Hubert
*/

#include <immintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "test_lib.h"
#include "fr_utils.h"
#include <signal.h>


#define CALIB_TRY 100
#define READ_END 0
#define WRITE_END 1
#define STDOUT 1

int nb = 0;

/**
 * calibration function
 */
size_t fr_probethreshold (void) {
  static char dummy;
  size_t res_a = 0;
  size_t res_n = 0;
  for (int i = 0; i < CALIB_TRY; i++) {
    _mm_clflush(&dummy);
    res_a += memaccesstime(&dummy);
  }

  for (int i = 0; i < CALIB_TRY; i++) {
    res_n += memaccesstime(&dummy);
  }

  res_a /= CALIB_TRY;
  res_n /= CALIB_TRY;
  return (res_a + res_n) / 2;
}

void sigint_handler (int sig) {

  /* print nb acces trace before exit */
  printf("\nNb of access: %d\n", nb); 
  exit(0);
}

int main (void) {

  signal(SIGINT, sigint_handler);

  void * addr = map_offset("test/target.txt", 0);

  size_t threshold = fr_probethreshold();

  printf("probe time: %ld delta < probe = access\n", threshold);

  flush_and_reload(addr);

  int access = 0;
  while (1) {
    
    access = 0;
    int64_t delta = flush_and_reload(addr);
    if (delta < threshold) {
    	printf("delta: %li, access ! \n", delta);
      nb++;
      access = 1;
    }
    write(STDOUT, &access, sizeof(int));
    delayloop(10000);
  }
  return 0;
}
