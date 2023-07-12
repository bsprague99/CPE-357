#include "mush2.h"
#define READ 0
#define WRITE 1

void sig_catch(int signum) {
  if (signum == SIGINT)
   printf("\n");
}

int main(int argc, char *argv[]) {
  FILE *input = stdin;
  char *line;
  pipeline pl;
  int i, run, status, in, out, err=0;
  run = TRUE;
  int **pipe_arr;
  int *temp_pipe;
  int child_count = 0;
  pid_t child;
  in = STDIN_FILENO;
  out = STDOUT_FILENO;
  int PROCS = 0;


  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sig_catch;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);


  printf("8-P ");
  while (run) {

    if(argc == 2) {
      if(!(input = fopen(argv[1], "r"))) {
        perror(argv[1]);
        run = FALSE;
        }
      else {
        input = stdin;
      }
   }

  if (NULL == (line = readLongString(input)) ) {
    if (feof(input)) {
        run = FALSE;
    }
    printf("8-P ");
  }
  else {
    if (strncmp(line, "", 1) == 0){
      printf("8-P ");
      continue;
    }
      if(!(pl = crack_pipeline(line))){
        perror("crackpipe");
        exit(EXIT_FAILURE);
      }

    PROCS = pl->length;
    pipe_arr = (int**)malloc(sizeof(int*) * (PROCS - 1));

    for(i = 0; i < PROCS - 1; i++) {
      temp_pipe = (int*)malloc(sizeof(int) * 2);
        pipe(temp_pipe); //error check
        pipe_arr[i] = temp_pipe;
      }
      if ( pl != NULL ) {
        //meat
        if(!strcmp(pl->stage->argv[0], "cd")) {
          if(chdir(pl->stage->argv[1]) < 0)
          perror(pl->stage->argv[1]);
        continue;
      }
    }

    for(i=0; i < PROCS; i++) {
      child_count++;
      if ((child = fork()) == 0) {
          /* child stuff*/
          /* figure out input and outputs */

        if (i == 0) {
          in = STDIN_FILENO;
        }
        else{
          in = pipe_arr[i - 1][READ];
        }
        if ( i < PROCS - 1) {
          out = pipe_arr[i][WRITE];
        }
        else {
          out = STDOUT_FILENO;
        }
        /* do the dups */
        if (in != STDIN_FILENO){
          if(-1 == dup2(in, STDIN_FILENO)){
            perror("dup2");
            exit(EXIT_FAILURE);
          }
          close(in);
        }
        if (out != STDOUT_FILENO){
          if(-1 == dup2(out, STDOUT_FILENO)){
            perror("dup2");
            exit(EXIT_FAILURE);
          }
          close(out);
        }
          /* close extra file descriptors */
        for (i =0; i < PROCS -1; i++){
          close(pipe_arr[i][READ]);
          close(pipe_arr[i][WRITE]);
        }
        execvp(pl->stage[i].argv[0], pl->stage[i].argv);
      }
      else{
          /* parent */
        if ( child == -1) {
          perror("fork()");
          exit(EXIT_FAILURE);
        }
      }
    }

    for (i =0; i < PROCS -1; i++){
      close(pipe_arr[i][READ]);
      close(pipe_arr[i][WRITE]);
    }
      /* wait for children */
    for (i =0; i < child_count; i++){
      if (-1 == wait(&status)) {
        perror("wait");
        exit(EXIT_FAILURE);
      }
    }
    fflush(stdout);
    if (!WIFEXITED(status) || WEXITSTATUS(status))
      err++;
    

    //print_pipeline(stdout,pl); /* print it. */
    
    free(line);
    free_pipeline(pl);
    yylex_destroy();
    child_count = 0;
    }
    
    if (run){  
      fflush(stdout);
      printf("8-P ");
    }
  }
  return 0;
}


   
