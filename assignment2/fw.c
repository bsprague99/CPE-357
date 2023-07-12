#include<"fw.h">

/*search for a word that already exists or not*/
tree_node *tree_search(tree_node *t, char *word) {
    if (t != NULL) {
        if (strcmp(t->word, word) == 0) {
            return t;   
        }  
        else {
            tree_node *located = tree_search(t->left, word);
            if (located == NULL) {
                located = tree_search(t->right, word);
            }
            return located;
        }
    }
    else {
        return NULL;
    }
}


int count(tree_node *root){
    if(root == NULL){
        return 0;
    }
    else{
        return 1 + count(root->left) + count(root->right);
    }
}

/*decide whether or not to update a 
freq of existing node, or add a new node*/
tree_node *insert(tree_node *t, char *word) {
    if (t == NULL) {
        printf("creating node for:");
        printf(word);
        return create_node(word);
    }
    if (t->freq > 1) {
        t->left = insert(t->left, word);
    }
    else {
        t->right = insert(t->right, word);
    }
    return t;
}

/* malloc and set tree attributes to null*/
tree_node *create_node(char *word) {
    tree_node *node;
    if ((node = malloc(sizeof(tree_node) * 1)) == NULL) {
        perror("malloc: failed\n");
        exit(-1);
    }

    node->word = word;
    node->freq = 1;
    node->left = NULL;
    node->right =  NULL;

    return node;
}

/*passes off words to program to place in a 
tree or update an existing node*/
void pass_to_tree(FILE *file, tree_node *t){
    char *word;
    tree_node *node;
    if ((node = malloc(sizeof(tree_node) * 1)) == NULL) {
        perror("malloc: failed\n");
        exit(-1);
    }
    int count = 0;

    while((word = read_word(file))) {
        if (count == 0){
            t = insert(t,word);
            count++;
            continue;
        }
        printf("got this word:");
        printf(word);
        printf("\n");
        node = tree_search(t, word);
        //printf("pls");
        if (node != NULL) {
            printf("updatinfcoun");
            node->freq += 1;
        }
        else {
            //printf("we didnt match\n");
            t = insert(t, word);
            free(word);
        }
     }
}


void print_tree(tree_node *t, int num, int i) {
    if (t == NULL) {
        //printf("here2\n");
        return;
    }

    print_tree(t->left, num, i);
    if (i++ < num) {
        printf("%d", t->freq);
        printf(" ");
        printf("%s", t->word);
        printf("\n");
    }
    print_tree(t->right, num, i);
    
}

char *read_word(FILE *in) {

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
  while (((c=getc(in))!=EOF) && (isalpha(c) != 0)) {
    /* make sure there's space */
    if ( i > size - 2 ) {
      size += CHUNK;
      if( !(s=realloc(s,size)) ) {
        perror("realloc");
        exit(EXIT_FAILURE);
      }
    }
    /* put it in */
    s[i++] = tolower(c);
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
    int num = 10;
    FILE *fp;
    tree_node *t = NULL;    
    if (argc == 1) {
        perror("usage: fw [-n num] [ file1 [file 2 ...]]\n");
        return 1;
    }

    if(strcmp(argv[1],"-n") == 0) {
        if(!atoi(argv[2])){
            perror("usage: was not a number\n");
            return 1;
        }
        else { 
            if (argc > 3) {

                int i;
                for (i = 3; i<argc; i++) {
                    if (fopen(argv[i], "r") == NULL) {
                        perror("nonexistant: No such file or directory\n");
                    } else {
                        fp = fopen(argv[i], "r");
                        printf("here9\n");
                        pass_to_tree(fp, t);
                        fclose(fp);
                    }
                }
            }else {
                if (argc == 3) {
                printf("here10\n");

                pass_to_tree(stdin, t);
                fclose(stdin);
                }
            }
        }
    }  
     
    else {

        if (argc >= 2) {
            int i;
            for (i = 1; i<argc; i++) {
                if (fopen(argv[i], "r") == NULL) {
                    perror("nonexistant: No such file or directory\n -n ");
                } else {
                    fp = fopen(argv[i], "r");
                    pass_to_tree(fp, t);
                    fclose(fp);
                }
            }
        }
        else if (argc == 1) {
            pass_to_tree(stdin, t);
        }
    }
    printf("The top %d words (out of %d) are:\n", num, count(t));
    print_tree(t, num, 20);
    //free_tree(t);
    return 0;
}
