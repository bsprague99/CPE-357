#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

/* Delete function handles -c 
 * -d and -d (del compliment/delete) 
 *     Mode 1 is delete if x exists 
 *        in set, then delete it
 *           mode 0 is for delete 
 *              compliment prints everything 
 *                 but items in set*/
void delete_function(char *set, int mode)
{
    int x;
    int index;
    while((x = getchar()) != EOF)
    {
        char *loc;
        loc = rindex(set, x);
        if (mode == 1) { 
            if (loc == NULL){
                putchar(x);
            }
        }
        else {
            index = loc - set;
            if (loc != NULL){
                if (index != strlen(set)){
                    putchar(x);
                }
                
            }
        }
    }
}


void translate_function(char *set1, char *set2)
{
    int x;
    int index;
    while((x = getchar()) != EOF) { /* open up stdinout*/
        char *loc;
        loc = rindex(set1, x);
        
        if (!loc) { 
            putchar(x);
        }
        else {
            index = loc - set1;
            if (index == strlen(set1)) { /* null char case*/
                putchar(x);
            }
            else if ( index >= strlen(set2)) {
                putchar(set2[strlen(set2) - 1]);
            }
            else {
                putchar(set2[index]);
            }
        }
    }
    /*printf("\n");*/
}




int main(int argc, char *argv[]) {

    /*toss out anything over 4 or less than 3 */
    if (argc > 4 || argc < 3)
    {
        fprintf(stderr,"usage: ./xlat [ -c ] [ -d ] set1 [ set2 ]\n");
        return 1;
    }

    /* check that the number and contents of args are correct 
 *         checks for a -c -d*/
    if (strcmp("-c", argv[1]) == 0)
    { 
        if (strcmp(argv[2], "") == 0) { 
         return 0;
     }

        if (strcmp("-d", argv[2]) == 0 && argc == 4)
        {
            delete_function(argv[3], 0);
        }

        else
        {
            fprintf(stderr, "usage: ./xlat [ -c ] [ -d ] set1 [ set2 ]\n");
            return 1;
        }
    }
    /* checks for a -d -d */
    else if (strcmp("-d", argv[1]) == 0)
    {
        
        if (strcmp("-c", argv[2]) == 0 && argc == 4)
        {
        
            delete_function(argv[3], 0);
        }

        else if (strcmp("-c", argv[2]) && argc == 3)
        {
            delete_function(argv[2], 1);
        }

        else
        {
            fprintf(stderr, "usage: ./xlat [ -c ] [ -d ] set1 [ set2 ]\n");
            return 1;
        }
    }
    else
        /* translate mode with no flags */
    {
        if (strcmp(argv[2], "") == 0) { 
         fprintf(stderr, "set2 can't have zero "\
             "length in translate mode.\n");          
         fprintf(stderr,"usage: ./xlat [ -c ] [ -d ] set1 [ set2 ]\n");
         return 1;
     }
     else {
      translate_function(argv[1], argv[2]);
  }
}
return 0;
}

