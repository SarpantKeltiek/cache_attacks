#define _GNU_SOURCE

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "test_lib.h"
#include "time.h"
#include "signal.h"
#include "sys/types.h"
#include <sched.h>
#include <sys/mman.h>
#include <fcntl.h>

static int count;

int memaccess(void *v)
{
	int rv;
	asm volatile("mov (%1), %0"
				 : "+r"(rv)
				 : "r"(v)
				 :);
	return rv;
}

/* memory acces to addr */
int func_to_spy(void *addr)
{

	while (1) {

		usleep(100000);			/* sleep 100ms */
		int test = rand() % 20; /* 5% of chance to access memory */
		if (test == 0) {
			memaccess(addr);
			count++;
		}

		fake_encrypt(1, 2);
	}
}

/* mastik file mapping function with offset */
void *map_offset(const char *file, uint64_t offset) {
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

void sig_handler (int sign) {

	if (sign == SIGINT) {
		printf("Count of lib's function call: %i\n", count);
    exit(EXIT_SUCCESS);
	}
}

int main() {

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(2, &mask); /* we chosse ce cpu 0 for trojan */

  sched_setaffinity(0, sizeof(cpu_set_t), &mask); /* set the cpu mask */

  signal(SIGINT, sig_handler);

	srand(time(NULL));
	printf("%s\n", "test");
	int test = (int) sqrt(9);
	//printf("%p", system);

  void * addr = map_offset("test/secret.txt", 0);

  func_to_spy(addr);
}
