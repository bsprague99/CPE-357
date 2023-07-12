#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>
#define BUFFER_MAX 1000

char *read_long_line(FILE *file){
	char *block;
	int counter = 0;
	char c;

    block = malloc(BUFFER_MAX * sizeof(char));

    while((c = fgetc(file)) != EOF){
        block[counter] = c;
    	if (c == '\n'){
            counter++;
            block[counter] = '\0';
    		return block;
    	}

        counter++;
    	
    	if (counter == BUFFER_MAX){
    		block = realloc(block, BUFFER_MAX * sizeof(char));
    	}

    	

    }
    free(block);
    return NULL;
}


int main(int argc, char *argv[]) {

	char *line1 = read_long_line(stdin);
	char *line2;

	while(line1 != NULL){
		line2 = read_long_line(stdin);
		if (line2 == NULL){
			break;
		}
		if (strcmp(line1, line2)){
			printf("%s", line1);
		}
		line1 = line2;
		/*free(line2); */


	}
	printf("%s", line1);
	free(line1);
	free(line2);

}
