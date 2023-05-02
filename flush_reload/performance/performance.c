/*
@author: Sarpant
*/

#include <immintrin.h>
#include <emmintrin.h>
#include <mmintrin.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

#include <signal.h>

#define READ_END 0
#define WRITE_END 1

size_t v_access = 0; /* true access */
size_t p_access = 0; /* predict access */

size_t faux_p;
size_t faux_n;
size_t total_g_predict;
size_t total_predict;
size_t total_access;

int time_to_wait;

FILE * res_fp;

void handle_victime_sig (int sig) {
    
  v_access++;
  if (v_access > 1) {
    faux_n++;
    //puts("Faux negatif");
    v_access = 1;
  }
  total_access++;
}

/* launch the victim */
pid_t launch_victim(char *envp[], char * time_to_wait) {

  pid_t pid = fork();
  
  if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid)
    return pid;

  int envc = 0;
  while (envp[envc] != NULL) {
    envc++;
  }

  char *new_envp[envc + 2];
  for (int i = 0; i < envc; i++) {
    new_envp[i] = envp[i];
  }

  new_envp[envc] = "LD_LIBRARY_PATH=./lib/";
  new_envp[envc + 1] = NULL;

  char *args[] = {"./victim", time_to_wait, NULL};
  usleep(100000);
  execve(args[0], args, new_envp);
  perror("execve");
  exit(EXIT_FAILURE);
}

/* launch the attack */
pid_t launch_attack(int pipe_fd[]) {
  
  pid_t pid = fork();
  if (pid == -1)
  {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (pid)
    return pid;

  /* exec the attack and pipe the ouput to recev message from attacker */
  close(pipe_fd[READ_END]);
  close(1);
  dup(pipe_fd[WRITE_END]);

  char *args[] = {"./flushreload", NULL};
  execve(args[0], args, NULL);

  perror("execve");
  exit(EXIT_FAILURE);
}

void resume(int time_to_wait) {
  printf("---> Résumé: \n");
  printf("Total du nombre d'access: %ld\n", total_access);
  printf("Total des accès prédits: %ld\n", total_predict);
  printf("Total des accès correctement prédits: %ld\n", total_g_predict);
  printf("Faux positif: %ld\n", faux_p);
  printf("Faux negatif: %ld\n", faux_n);

  fprintf(res_fp, "%d,%ld,%ld,%ld,%ld,%ld\n", time_to_wait, faux_p, faux_n, total_g_predict, total_predict, total_access);
}

/* handler for ctrl+c */
void handle_sig(int sig) {
  resume(time_to_wait);

  if (res_fp != NULL)
    fclose(res_fp);

  exit(EXIT_SUCCESS);
}

int benchmark(size_t access_train, char * time_to_wait, char *envp[]) {

  signal(SIGINT, handle_sig);

  int num_child = 2;
  int pipe_fd[2];


  if (pipe(pipe_fd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }
  
  pid_t pid_attack = launch_attack(pipe_fd);
  pid_t pid_victim = launch_victim(envp, time_to_wait);

  signal(SIGUSR1, handle_victime_sig); /* handler for victim signal */

  close(0);
  close(pipe_fd[1]);
  dup(pipe_fd[0]);

  while (total_access < access_train) {
    /* recv message from attacker */
    read(pipe_fd[0], &p_access, sizeof(size_t));

    /* if access predicted by attackant */
    if (p_access == 1)
    {

      total_predict++;
      if (v_access == 1) { /* good prediction */
        total_g_predict++;
        p_access = 0;
        v_access = 0;
      }

      else if (v_access == 0) { /* false positive */
        faux_p++;
        p_access = 0; /* reset prediction*/
      }
    }
  }

  /* wait for the end of the victim */
  kill(pid_victim, SIGUSR1);
  waitpid(pid_victim, NULL, 0);

  kill(pid_attack, SIGUSR1);
  waitpid(pid_attack, NULL, 0);
}

void reset() {
  v_access = 0;
  p_access = 0;
  faux_p = 0;
  faux_n = 0;
  total_g_predict = 0;
  total_predict = 0;
  total_access = 0;
}

int main(int argc, char *argv[], char *envp[]) {

  if (argc != 3) {
    printf("Usage: %s <access_train> <time_limit>\n", argv[0]);
    return 1;
  }

  size_t access_train = atoi(argv[1]);
  size_t time_limit = atoi(argv[2]);


  res_fp = fopen("result.csv", "w");
  if (res_fp == NULL) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }

  fprintf(res_fp, "time_to_wait,faux_p,faux_n,total_g_predict,total_predict,total_access\n");

  char time_to_wait[10];

  /* test benchmark with different time to wait */
  for (size_t i = 1; i < 10000000; i = i * 2) {

    reset();
    printf("\n\n\n----- Benchmark with time to wait: %ld ----- \n", i);
    sprintf(time_to_wait, "%ld", i);
    benchmark(access_train, time_to_wait, envp);
    resume(i);
  }

  fclose(res_fp);

  return 0;
    
}