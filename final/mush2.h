#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include "mush.h"
#include <sys/wait.h>

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (0==1)
#endif

// struct clstage {
//   char *inname;              /* input filename (or NULL for stdin)      */
//   char *outname;             /* output filename (NULL for stdout)       */
//   int   argc;                /* argc for the stage                      */
//   char **argv;               /* Array for argv (NULL Terminated)        */

//   clstage next;              /* link pointer for listing in the parser  */
// };

// typedef struct pipeline {
//   char *cline;              /* the original command line                */
//   int length;                length of the pipeline                   
//   struct clstage *stage;    /* descriptors for the stages               */
// } *pipeline;

void sig_catch(int signum);
int check_empty(char *pipeline);
