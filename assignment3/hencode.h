#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define ONE_BYTE 8
#define MAX_BUFF 4096
#define HEADER 4
#define SIZE 256

typedef struct HuffmanNode htable;

typedef struct HuffmanNode {
    unsigned char my_char;
    int count;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
}HuffmanNode;

HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr);
int comparator(const void *a, const void *b);
void combine_tree(HuffmanNode **head, int *cptr);
void encode(HuffmanNode *head, unsigned char *codes[], char c[]);
void formatcodes(int fout, int fin, unsigned char *codes[], int *cptr);
int conversion(char *traversals);
void free_htable(HuffmanNode *head);
int comparator_tie(const void *a, const void *b);
