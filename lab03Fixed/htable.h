#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>

#define SIZE 256

typedef struct HuffmanNode {
 
    unsigned char my_char;
    int count;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
}HuffmanNode;

HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr);
int comparator(const void *a, const void *b);
void combine_tree(HuffmanNode **head, int *cptr);
void encode(HuffmanNode *head, unsigned char *codes[],char c[]);
void free_htable(HuffmanNode *head);
