#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	pid_t child;
	int status;
	if (argc == 2) {
		if ((child = fork())) {
			if (child == -1) {
				perror("fork");
				exit(1);
			}
			if (wait(&status) ==  -1) {
				perror("wait");
				exit(1);
			}
			if (!WIFEXITED(status) || WEXITSTATUS(status)) {
				exit(EXIT_FAILURE);
			}
			else {
				exit(-1);
			}
		}
		else {
			execl(argv[1], argv[1], NULL);
			perror(argv[1]);
			exit(3);
		}
	}
	else {
		printf("usage: tryit command\n");
		exit(-1);
	}
	return 1;
}