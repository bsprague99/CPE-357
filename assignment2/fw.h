#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<ctype.h>

#define CHUNK 80 

typedef struct tree_node{
	char *word;
	int freq;
	struct tree_node *left;
	struct tree_node *right;
}tree_node; /*created alias for structure tree_node thats called tree_node*/


void pass_to_tree(FILE *file, tree_node *t);
char *read_word(FILE *file);
tree_node *tree_search(tree_node *t, char *word);
tree_node *create_node(char *word);
tree_node *insert(tree_node *t, char *word);
void print_tree(tree_node *t, int num, int i);
//void free_tree(tree_node *t);
