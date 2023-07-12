#include"htable.h"

int main(const int argc, const char *argv[]) {
    int c = 0; //initilize a variable for the character
    int counter = 0; //intilize a var for the number of nodes created
    int *cptr = &counter;
    int i; //var for for loop iteration
    int ascii[SIZE];
    int x;
    int y;
    unsigned char *codes[SIZE] = {0}; 
    HuffmanNode **head = NULL; //initilize a pointer that 
    FILE *fp = NULL; 
    char location[SIZE] = {0};

    for (x = 0; x < SIZE; ++x) {
        ascii[x] = 0;
    }    

    if((fp = fopen(argv[1],"r"))) { //open file

        while((c = getc(fp)) != EOF) { //parse through file
            //convert the char to ascii and then add one to that chars index
            ascii[(int)c] += 1; 

        }
        //place to store number of nodes created
        head = build_tree(ascii, head, cptr ); 
        //sort these nodes in ascending order
        qsort(head, counter, sizeof(HuffmanNode*), comparator); 
        combine_tree(head, cptr);
        if (counter != 0) {
            encode(head[0], codes, location);
        }

        for (i = 0; i < SIZE; i++) {
            //printf("%s\n", codes[i]);
            if (ascii[i] != 0) {
                fprintf(stdout, "0x%02x: %s\n", i, (unsigned char *)codes[i]);
            }
        }
    }
    else {
        perror(argv[1]);
        fclose(fp);
        return 1;
    }
    free_htable(head[0]);
    for (y = 0; y < SIZE; y++) {
        free(codes[y]);
    }
    free(head);
    fclose(fp);
    return 0;
}

HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr) {
    int i;
    HuffmanNode *node = NULL; 

    if ((head = malloc(SIZE * sizeof(HuffmanNode*))) == NULL) {
        perror("malloc: failed\n");
        exit(-1);
    }

    //go through ascii list and make a node for each char
    for (i = 0; i < SIZE; i++) { 
        if (ascii[i] != 0) {
            if ((node = malloc( 1 * sizeof(HuffmanNode))) == NULL) {
                perror("malloc: failed\n");
                exit(-1);
            }
            node->my_char = (char)i;
            node->count = ascii[i];
            node->left = NULL;
            node->right =  NULL;

            head[*cptr] = node;
            (*cptr)++;
        }
    }
    return head;
}

int comparator(const void *a, const void *b) { //sorting method

    HuffmanNode* huff_a = (*(HuffmanNode**)a);
    HuffmanNode* huff_b = (*(HuffmanNode**)b);


    if (!huff_a && !huff_b)
        return 0;
    if (!huff_a)
        return 1; // or -1
    if (!huff_b)
        return -1; // or 1

   if (huff_a->count == huff_b->count) { //tie breaker 
    //put char1 after in ascending order
        //if (huff_a->my_char > huff_b->my_char) { 
          //  return 1;
        //} else {
          //  return -1;
        //}
       return -1;
   }
   else if (huff_a->count < huff_b->count) {
      return -1;
   }
   else {
      return 1;
   }
}

void combine_tree(HuffmanNode **head, int *cptr) {

    HuffmanNode *new_node = NULL;
    HuffmanNode *child_1 = NULL;
    HuffmanNode *child_2 = NULL;

    int i;

    for (i = *cptr; i > 1; i--) {
        
        if ((new_node = malloc(sizeof(HuffmanNode))) == NULL) {
            perror("malloc: failed\n");
            exit(-1);
        }
        child_1 = head[0];
        child_2 = head[1];

        if (child_1->my_char < child_2->my_char) {
            new_node->my_char = child_1->my_char;
        }
        else {
            new_node->my_char = child_2->my_char;
        }

        new_node->left = child_1;
        new_node->right = child_2;
        new_node->count = child_1->count + child_2->count;

        head[0] = new_node;
        head[1] = NULL;
        qsort(head, *cptr, sizeof(HuffmanNode*), comparator);
    }
}

void encode(HuffmanNode *head, unsigned char *codes[], char c[]) {

    if ((head->right == NULL) && (head->left == NULL)) {
        codes[head->my_char] = (unsigned char *)strdup(c);
        
        c[strlen(c) - 1] = '\0';
    }
    else {
        //printf("%c", head->my_char);
        encode(head->right, codes, strcat(c, "1"));

        encode(head->left, codes, strcat(c, "0"));
        
        c[strlen(c) - 1] = '\0';
    }
}

void free_htable(HuffmanNode *head) {//recursivly free nodes in the tree
    if (head != NULL) {
        if ((head->left != NULL) && (head->right != NULL)) {
            free_htable(head->left);
            free_htable(head->right);
            free(head);
        } else {
            free(head);
        }
    }
}

