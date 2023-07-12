/* baspragu */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#define TICK 7
#define TOCK 5


void handle_it() {
   static int i = 1;
   if (i) {
      write(STDOUT_FILENO, "Tick...", TICK);
   }
   else {
      write(STDOUT_FILENO, "Tock\n", TOCK);
   }
   i = !i;
}

int main(int argc, char **argv) {
   int timer_num;
   int i;
   int x;
   struct sigaction sa;
   struct itimerval timer;
   sa.sa_handler = handle_it;

   if (argc > 2 || argc < 2) {
      printf("usage: timeit <seconds>\n");
      exit(-1);
   }
   for (x = 0; x < strlen(argv[1]); x++) {
      if (isdigit((argv[1][x])) == 0) {
         printf("Malformed time\n");
         printf("usage: timeit <seconds>\n");
         exit(-1);
      }
   }

   timer_num = atoi(argv[1]);
   i = timer_num * 2;
   sigfillset(&sa.sa_mask);
   sigdelset(&sa.sa_mask, SIGALRM);

   sa.sa_flags = 0;

   timer.it_interval.tv_sec = 0;
   timer.it_interval.tv_usec = 500000;
   timer.it_value.tv_sec = 0;
   timer.it_value.tv_usec = 500000;

   sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);
   
   sigaction(SIGALRM, &sa, NULL);
      
   setitimer(ITIMER_REAL, &timer, NULL);

   while (i) {
      pause();
      i--;
   }
   printf("Time's up!\n");
   return 0;
}


