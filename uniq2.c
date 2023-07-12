#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>
#include<unistd.h>
#define CHUNK  100
/*
 * char *read_long_line(FILE *file){
 *  char *block;
 *      int counter = 0;
 *          char c;
 *
 *              block = malloc(BUFFER_MAX * sizeof(char));
 *
 *                  while((c = fgetc(file)) != EOF){
 *                          if (c == '\n'){
 *                                      block[counter] = '\0';
 *                                                  counter++;
 *                                                              return block;
 *                                                                      }
 *
 *                                                                              block[counter] = c;
 *                                                                                      counter++;
 *                                                                                              
 *                                                                                                      if (counter == BUFFER_MAX){
 *                                                                                                                  block = realloc(block, BUFFER_MAX * sizeof(char));
 *                                                                                                                          }
 *
 *                                                                                                                                  
 *
 *                                                                                                                                      }
 *                                                                                                                                          free(block);
 *                                                                                                                                              return NULL;
 *                                                                                                                                              }
 *                                                                                                                                              */




char *read_word(FILE *in) {
  /* read a long line from in and store it in a newly-allocated buffer.
 *    * on success returns returns the string.  On EOF w/o finding a string
 *       * returns NULL.  Does not return on error.
 *          * trims newlines, too, cuz good.  (Come back and edit this so later
 *             * programmers don't think I'm illiterate)
 *                */
  char *s;
  int c;
  size_t size, i;


  /* get a hunk */
  size = CHUNK;
  if ( !(s = malloc(size)) ) {
    perror("malloc");
    exit(EXIT_FAILURE);
  }

  /* read into it until EOF or \n */
  i = 0;                        /* start at beginning of string (why not?) */
  while ( ((c=getc(in))!=EOF) && (isalpha(c) != 0) ) {
    /* make sure there's space */
    if ( i > size - 2 ) {
      size += CHUNK;
      if( !(s=realloc(s,size)) ) {
        perror("realloc");
        exit(EXIT_FAILURE);
      }
    }
    /* put it in */
    s[i++]=c;
  }
  s[i++]='\0';

  /* adjust to the right size */
  if( !(s=realloc(s,i)) ) {
    perror("realloc");
    exit(EXIT_FAILURE);
  }

  /* check for EOF */
  if ( c==EOF && i == 1 ) {
    free(s);
    s = NULL;
  }

  return s;
}


int main(int argc, char *argv[]) {

    char *line1 = read_word(stdin);
    char *line2;

    while(line1 != NULL){
        line2 = read_word(stdin);
        if (line2 == NULL){
            break;
        }
    
            printf("%s", line1);
            printf("\n");
        line1 = line2;
        /*free(line2); */


    }
    printf("%s", line1);
    free(line1);
    free(line2);

}

