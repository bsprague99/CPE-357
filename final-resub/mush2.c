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
  int *temp_pipe;
  int child_count = 0;
  pid_t child;
  in = STDIN_FILENO;
  out = STDOUT_FILENO;
  int PROCS = 0;
  int fd[2];

  /****signal handling*****/
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sig_catch;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);
 
  /* handles ./mush2 foofile */
  if(argc == 2) {
      if(!(input = fopen(argv[1], "r"))) {
        perror(argv[1]);
      }
  }
  /* meat of the program...handles the shell stuff*/
  while (run) {

    if(isatty(fileno(input)) && isatty(STDOUT_FILENO))
        printf("8-P ");

    if (NULL == (line = readLongString(input))) {
      if (feof(input)) {
          run = FALSE;
          break;
      }
    }

    else {
      /* handles the newlinw prompting */
      if (strncmp(line, "", 1) == 0){
        continue;
      }
      if(!(pl = crack_pipeline(line))){
        perror("crackpipe");
        exit(EXIT_FAILURE);
      }

      PROCS = pl->length;

      /* utilize a 2darray to handle in/out/plumbing */
      int all_fds[PROCS][2];

      all_fds[0][0] = dup(STDIN_FILENO);
      if ( pl != NULL ) {
        /* meat */
        /* outname == NULL STD case */
        if (pl->stage[PROCS-1].outname == NULL){
            all_fds[PROCS-1][1] = dup(STDOUT_FILENO);
        }
        /* CD check, as well as a check for CD without a path */
        if(!strcmp(pl->stage->argv[0], "cd")) {
          if (pl->stage->argc == 1) {
            if (chdir(getenv("HOME")) < 0) {
              perror("chdir");
            }
          }
          else if (chdir(pl->stage->argv[1]) < 0)
            perror(pl->stage->argv[1]);
          continue;
        }
        for(i = 1; i < PROCS; i++) {
            pipe(fd);
            if (pl->stage[i-1].outname != NULL) {
              continue;
            }
            /* set plumbing for Read/Write */
            all_fds[i][0] = fd[0];
            all_fds[i - 1][1] = fd[1];
        }

        for(i=0; i < PROCS; i++) {
          child_count++;
          if ((child = fork()) == 0) {
              /* child stuff*/
              /* figure out input and outputs */
          /* for case where redirection is used */
          if (pl->stage[i].outname != NULL) {
            if ((all_fds[i][1] = open(pl->stage[i].outname, 
              O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1) {
                perror("Cannot open output file\n"); 
                exit(1);
            }
          }
          if (pl->stage[i].inname != NULL) {
            if ((all_fds[i][0] = open(pl->stage[i].inname, O_RDONLY)) == -1) {
                perror("Cannot open input file\n"); 
                exit(1);
            }
          }
          /* do the dups! */
          dup2(all_fds[i][0], STDIN_FILENO);
          dup2(all_fds[i][1], STDOUT_FILENO);
          close(all_fds[i][0]);
          close(all_fds[i][1]);
          /* remember to exec after closing */
          execvp(pl->stage[i].argv[0], pl->stage[i].argv);
          exit(-1);
          }
          else{
            close(all_fds[i][0]);
            close(all_fds[i][1]);
            waitpid(child, &status, 0);
            /* parent */
            // if ( child == -1) {
            //   perror("fork()");
            //   exit(EXIT_FAILURE);
            // }
            fflush(stdout);
          }
        }
      //print_pipeline(stdout,pl); /* print it. */
      free(line);
      free_pipeline(pl);
      child_count = 0;
      }
    }  
  }
  yylex_destroy();
  return 0;
}
