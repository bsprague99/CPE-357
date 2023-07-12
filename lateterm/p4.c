/* baspragu */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

int same_file(const char *src, const char *dest) {

	struct stat src_st;
	struct stat dest_st;
	/************** using the mem addr of struct **************/
	if (stat(src, &src_st) != 0 || stat(dest, &dest_st) != 0) {
		perror("stat");
		exit(1);
	}
	/********************************************************/
	/*  If two files have the same deviceID and inode,		*/
	/*  they're the same file (as outlined in previous		*/
	/*  course work). The stat function lets you obtain		*/
	/*  the inode and and device ID from the struct, and	*/
	/*  comparing them lets you see if the paths are		*/
	/*  pointing to the same file or not.       			*/
	/********************************************************/
	if ((dest_st.st_dev == src_st.st_dev) && (dest_st.st_ino == src_st.st_ino)) {
		return 1;
	}
return 0;
}
int main(int argc, char *argv[]){
	int outcome = same_file(argv[1], argv[2]);
	if (outcome == 1) {
		printf("Files are identical\n");
	}
	else{
		printf("Files are not identical\n");
	}
	return 0;
}

/*
SAMPLE RUN:
brendensprague@Brendens-MacBook-Pro lateterm % ./a.out /Users/brendensprague/Desktop/lateterm/p3.c /Users/brendensprague/Desktop/lateterm/p3.c
Files are identical
brendensprague@Brendens-MacBook-Pro lateterm % ./a.out /Users/brendensprague/Desktop/lateterm/p3.c /Users/brendensprague/Desktop/lateterm/p5.c
Files are not identical
brendensprague@Brendens-MacBook-Pro lateterm %

As you can see, when comparing the paths/files of 3p.c to 3p.c, the function returned they are identical. 
When comparing p3.c to p5.c, the files were NOT identical!
*/