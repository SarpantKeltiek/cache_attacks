/*
  Author = Sarpant & Hubert
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>

#include <sys/mman.h>
#include <fcntl.h>
#include "test_lib.h"
#include "fr_utils.h"
#include "time.h"

#define CYCLES uint32_t

static int nb = 0;
static CYCLES cycles_to_wait = 0;
static int rd = 1;

/* memory acces to addr */
int func_to_spy(void * addr) {

	while(1) {

		delayloop(cycles_to_wait);
		int test = rand() % 1;
    if (test == 0) {
      memaccess(addr);
      kill(getppid(), SIGUSR1);
      nb++;
    }
	}
}

/* handler of sigint signal */
void sigint_handler(int sig) {

  exit(0);
}

int main(int argc, char const *argv[]) {

  if (argc >= 2) {
    cycles_to_wait = atoi(argv[1]);
  }

  if (argc >= 3) {
   
    rd = atoi(argv[2]);

  }

  if (rd <= 0) {
    rd = 1;
  }

  printf("current time: %d\n", cycles_to_wait);
  printf("rd: %d\n", rd);
  
  signal(SIGINT, sigint_handler);
  srand(time(NULL));

  void * addr = map_offset("test/target.txt", 0);
  int test = (int) sqrt(9);
  func_to_spy(addr);
}

