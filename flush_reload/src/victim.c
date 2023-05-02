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
// #include "../inc/test_lib.h"
#include "time.h"

static int nb = 0;
static int current_time = 1;

int memaccess(void *v) {
  int rv;
  asm volatile("mov (%1), %0": "+r" (rv): "r" (v):);
  return rv;
}

/* memory acces to addr */
int func_to_spy(void * addr) {

	while(1) {

		usleep(current_time);
		int test = rand() % 1;
    if (test == 0) {
      memaccess(addr);
      nb++;
    }

		//fake_encrypt(1, 2);
	}

}

/* mastik file mapping function with offset */
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


/* handler of sigint signal */
void sigint_handler(int sig) {

  /* print nb acces trace before exit */
  printf("\nNb of access: %d\n", nb);
  exit(0);
}

int main(int argc, char const *argv[]) {
  if (argc >= 2){
    current_time = atoi(argv[0]);
  }
  
    signal(SIGINT, sigint_handler);
    srand(time(NULL));

    void * addr = map_offset("test/target.txt", 0);
    int test = (int) sqrt(9);
    func_to_spy(addr);
}

