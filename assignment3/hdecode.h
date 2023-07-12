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

typedef struct HuffmanNode {
    unsigned char my_char;
    int count;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
}HuffmanNode;

HuffmanNode **build_tree(int ascii[], HuffmanNode **head, int *cptr);
void combine_tree(HuffmanNode **head, int *cptr);
void decode_htable(int fin, int fout, HuffmanNode *head, int *bitcount);
void encode(HuffmanNode *head, int *bitcount, unsigned char *codes[], char c[]);
void free_htable(HuffmanNode *head);
int comparator(const void *a, const void *b);
int comparator1(const void *a, const void *b);
void formatcodes(int fout, int fin, unsigned char *codes[], int *cptr);
void unconversion(unsigned char converted_code, char *traversals);
