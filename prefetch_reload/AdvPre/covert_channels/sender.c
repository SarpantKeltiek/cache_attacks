#define _GNU_SOURCE
#include "util.h"
#include "math.h"
#include "sched.h"
#include "pthread.h"
#include "signal.h"

int time_to_wait = 0;

void send_bit (bool bit, struct config *config)
{
  asm volatile("lfence");
  memaccess(config->addr);
}

void* sender_func(void* param) {

  printf("thead launched\n");
	struct config config;
	init_config(&config);

  //access the target data to make sure prefetchw works later.
  CYCLES access_time = measure_one_block_access_time(config.addr);
  access_time = measure_one_block_access_time(config.addr);

  int mm = 0;
	while (1) {

    usleep(time_to_wait);
    asm volatile("lfence");
    memaccess(config.addr);
	}
}



int main(int argc, char *argv[]) {
  
  pthread_t sender;
  printf("%s\n", "Starting\n");

  pthread_attr_t attr_sender;
  pthread_attr_init(&attr_sender);

  if (argc > 1) {
    time_to_wait = atoi(argv[1]);
  }

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(0, &mask);
  if (pthread_attr_setaffinity_np(&attr_sender, sizeof(mask), &mask) != 0) {
    perror("pthread_attr_setaffinity_np: sender");
  }

  printf("created thread\n");

  if (pthread_create(&sender, &attr_sender, (void *)sender_func, NULL) !=
      0) {
    perror("pthread_create: sender");
  }
  pthread_join(sender, NULL);
}







