#include"hdecode.h"

int ascii[SIZE];

/* Contains mostly shared code between hencode and decode, as well
 * as htable. Created two seperate header files once they needed their
 * own special methods */

int main(int argc, char *argv[]) {
	int fin;
	int fout;
	unsigned int read_cnt;
	unsigned char buff[MAX_BUFF];
	int i;
	int x;
	HuffmanNode **head = NULL; //initilize a pointer that 
	int counter = 0; //intilize a var for the number of nodes created
    int *cptr = &counter;
    char c[SIZE];
    unsigned char *codes[SIZE] = {0}; 
    unsigned char *my_char = NULL;
	unsigned int *count = NULL;
	int bitcount = 0;
    /* Assigning files, as well as setting permissions to in and out files. */
	if ((fin = open(argv[1], O_RDONLY)) < 0) { //fin file permission READONLY
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	if ((fout = open(argv[2],  O_TRUNC | O_CREAT | O_WRONLY, 
        S_IRUSR | S_IWUSR)) < 0) {
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}

	// while ((read_cnt = read(fin, buff, MAX_BUFF)) > 0) {
	// 	for (i = 0; i < read_cnt; i++)
	// 		ascii[buff[i]] += 1;
	// }
	
	if ((my_char = malloc(sizeof(unsigned char))) == NULL) {
        perror("Malloc: failed\n");
        exit(-1);
    }
    if ((count = malloc(sizeof(unsigned int))) == NULL) {
        perror("Malloc: failed\n");
        exit(-1);
    }
    //read to the input file frequency/char. Increment count pointer.
	read(fin, cptr, HEADER);
	int y;
	for (y = 0; y < *cptr; y++){
		read(fin, my_char, 1);
		read(fin, count, HEADER);
		ascii[*my_char] = *count;
	}
	free(my_char);
	free(count);

	if (*cptr == 1) {
		int i;
		for (i = 0; i < SIZE; i++) {
			if (ascii[i] > 0) {
				write(fout, &i, 1);
				close(fin);
				close(fout);
				return 0;
			}
		}
	}
    /* Node creation, tree creation, node sorting, tree sorting, comparator
     * are all performed here. */
	head = build_tree(ascii, head, cptr); 
    qsort(head, counter, sizeof(HuffmanNode*), comparator1); 
    combine_tree(head, cptr);
	encode(head[0], &bitcount , codes, c);
    /* decoding and freeing proccess */
	decode_htable(fin, fout, head[0], &bitcount);
	free_htable(head[0]);
	free(head);
	close(fin);
	close(fout);
	return 0;
}

//building nodes
HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr) {

    HuffmanNode *node = NULL; 

    if ((head = malloc(SIZE * sizeof(HuffmanNode*))) == NULL) {
        perror("malloc: failed\n");
        exit(-1);
    }
    int i;
    //go through ascii list and make a node for each char
    for (i = 0; i < SIZE; i++) { 
        if (ascii[i] != 0) {
            if ((node = malloc( 1 * sizeof(HuffmanNode))) == NULL) {
                perror("malloc: failed\n");
                exit(-1);
            }
            node->my_char = i;
            node->count = ascii[i];
            node->left = NULL;
            node->right =  NULL;

            head[*cptr] = node;
            (*cptr)++;
        }
    }
    return head;
}

//building tree
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

        if (child_1->my_char > child_2->my_char) {
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
//code extraction
void encode(HuffmanNode *head, int *bitcount, unsigned char
         *codes[], char c[]) {

    if ((head->right == NULL) && (head->left == NULL)) {
        c[strlen(c) - 1] = '\0';
        codes[head->my_char] = (unsigned char *)strdup(c);
        *bitcount += (ascii[(int)head->my_char] * 
        strlen(codes[(int)head->my_char]));
        
    }
    else {
        //printf("%c", head->my_char);
        encode(head->right, bitcount, codes, strcat(c, "1"));

        encode(head->left, bitcount, codes, strcat(c, "0"));
        
        c[strlen(c) - 1] = '\0';
    }
}
//comparator for build tree call
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
       //  if (huff_a->my_char > huff_b->my_char) { 
       //      return 1;
       //  } else {
       //      return -1;
       //  }
       // return 0;
    return -1;
   }
   else if (huff_a->count < huff_b->count) {
      return -1;
   }
   else {
      return 1;
   }
}
//comparator for the tie breaker between nodes in main
int comparator1(const void *a, const void *b) { //sorting method

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
        if (huff_a->my_char > huff_b->my_char) { 
            return 1;
        } else {
            return -1;
        }
       return 0;
    //return -1;
   }
   else if (huff_a->count < huff_b->count) {
      return -1;
   }
   else {
      return 1;
   }
}


void decode_htable(int fin, int fout, HuffmanNode *head, int *bitcount) {
	char *cbuff = NULL;
	char *chars = NULL;
	char *traversals = NULL;
	unsigned char *rbuff = NULL;
	int read_cnt = 0;
	int byte_size = 0;
	int this_char = 0;
	HuffmanNode *curr = head;
	
	int total = 0;

	if((traversals = calloc(ONE_BYTE, sizeof(char))) == NULL) { 
		perror("Calloc: failed.");
	    exit(-1);
	}
	if((cbuff = calloc(MAX_BUFF, sizeof(char))) == NULL ) { 
   	   perror("Calloc: failed.");
	   exit(-1);
	}	
	if((chars = calloc(MAX_BUFF, sizeof(char))) == NULL) { 
   		perror("Calloc: failed.");
	    exit(-1);
	}	
    if((rbuff = calloc(MAX_BUFF, sizeof(unsigned char))) == NULL) { 
	   perror("Calloc: failed.");
	   exit(-1);
	}	
	//read until read val is less than 0
	while ((read_cnt = read(fin, rbuff, MAX_BUFF)) > 0) {
		int i;
		for (i = 0; i < read_cnt; i++) {
            //unconvert and copy to char *
			unconversion(rbuff[i], traversals);
			strcpy((cbuff + byte_size), traversals);
			byte_size +=ONE_BYTE;
			int j;
            //traverse tree
			for (j = 0; j < byte_size; j++) {
				if (cbuff[j] == '0')
					curr = curr->left;
				else if (cbuff[j] == '1') {
					curr = curr->right;
				}
				if (curr->left == NULL && curr->right == NULL) {
					chars[this_char] = curr->my_char;
					this_char++;
					curr = head;
				}
				if (total + j + 1 == *bitcount) {
					write(fout, chars, this_char);
					free(chars);
					free(traversals);
					free(rbuff);
					free(cbuff);
					return;
				}
			}
			total += byte_size;
			byte_size = 0;
		}
	}
}
//convert code back
void unconversion(unsigned char converted_code, char *traversals){
	int i = 0;
	while (i < ONE_BYTE) {
		if (converted_code - pow(2, ONE_BYTE - 1 - i) < 0)
			traversals[i] = '0';
		else {
			traversals[i] = '1';
			converted_code -= pow(2, ONE_BYTE - 1 - i);
		} i++;
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




