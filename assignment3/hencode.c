#include"hencode.h"

int ascii[SIZE];

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
    
    /* Sets permissions for in and out files */

	if ((fin = open(argv[1], O_RDONLY)) < 0) { //fin file permission READONLY
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}

	if ((fout = open(argv[2],  O_TRUNC | O_CREAT | O_WRONLY, 
    S_IRUSR | S_IWUSR)) < 0) {
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}

	while ((read_cnt = read(fin, buff, MAX_BUFF)) > 0) {
		for (i = 0; i < read_cnt; i++)
			ascii[buff[i]] += 1;
	}

	head = build_tree(ascii, head, cptr); 
    qsort(head, counter, sizeof(HuffmanNode*), comparator_tie); 
    combine_tree(head, cptr);
    //null case
    if (head == NULL) {
	    lseek(fin, 0, SEEK_SET);
		write(fout, cptr, HEADER);
		for (x = 0; x < SIZE; x++) {
			if (ascii[x] > 0){
				write(fout, &x, sizeof(uint8_t));
				write(fout,(ascii + x), sizeof(uint32_t));
			}
	    }
		close(fin);
		close(fout);
		return 0;
	}

	encode(head[0], codes, c);
	formatcodes(fout, fin, codes, cptr);

    /* freeing process */
	free_htable(head[0]);
	free(head);
	close(fin);
	close(fout);
	return 0;
}

HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr) {
//builds huff nodes
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

int comparator(const void *a, const void *b) { //sorting method
//original comparator to be called in the tree creation method
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

int comparator_tie(const void *a, const void *b) { //sorting method
//this specific comparator was designed to complete the extra points
//tie breaker

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
   }
   else if (huff_a->count < huff_b->count) {
      return -1;
   }
   else {
      return 1;
   }
}

void combine_tree(HuffmanNode **head, int *cptr) {
//construction of the Huff tree. Also sorts too!
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

void encode(HuffmanNode *head, unsigned char *codes[], char c[]) {
//recursivly gets codes
    if ((head->right == NULL) && (head->left == NULL)) {
        c[strlen(c) - 1] = '\0';
        codes[head->my_char] = (unsigned char *)strdup(c);
        
    }
    else {
        //printf("%c", head->my_char);
        encode(head->right, codes, strcat(c, "1"));

        encode(head->left, codes, strcat(c, "0"));
        
        c[strlen(c) - 1] = '\0';
    }
}

void formatcodes(int fout, int fin, unsigned char *codes[], int *cptr){
	unsigned int num = 0;
	char *traversals = NULL;
	int wbuff = 0; 
	unsigned char buff[MAX_BUFF];
  	unsigned int read_cnt;
  	int i;
    int x = 0;
    lseek(fin, 0, SEEK_SET); //SEEK_SET sets file to beginning
	write(fout, cptr, HEADER);
	for (i = 0; i < SIZE; i++) {
		if (ascii[i] > 0){
			write(fout, &i, 1);
			write(fout, (ascii + i), HEADER);
		}
	}
	if((traversals = calloc(MAX_BUFF, sizeof(htable*))) == NULL) { 
	   perror("Calloc has failed.");
	   exit(-1);
	}
	
	while ((read_cnt = read(fin, buff, MAX_BUFF)) > 0) {
		while (x < read_cnt) {

			strcpy((traversals+num), codes[buff[x]]);
			num += strlen(codes[buff[x]]);

			if ((num % ONE_BYTE) == 0)  {
				traversals[num] = '\0';
                int j;
				for (j = 0; j < num / ONE_BYTE; j++) {
					wbuff = conversion(&(traversals[j * ONE_BYTE]));
					write(fout, &wbuff, 1);
				}
				num = 0;
			} x++;
		}
	}

    
	if ((num % ONE_BYTE != 0)) {
		int k = 0;
		while (k < ONE_BYTE - num) {
			traversals[num+k] = '0';
		}
		traversals[ONE_BYTE] = '\0';
		wbuff = conversion(traversals);
		write(fout, &wbuff, 1);
		k++;
	}
	free(traversals);

}

int conversion(char *traversals) { //converting traversal codes into bin
	int wbuff = 0;
    int i;
	for (i = 0; i < ONE_BYTE; i++){
		if (traversals[i] == '1')
			wbuff += pow(2, ONE_BYTE - i - 1);
	}
	return wbuff;
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
