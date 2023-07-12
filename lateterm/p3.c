/************************************************************/
/*  USAGE: This program accepts input through the 			*/
/*  command line! Enter your binary number as argv[1]		*/
/*  to get the decimal output. -Brenden Sprague         	*/
/************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>

int bstr2int(const char *s) {

	unsigned int dec = 0;
	int x = 0;
	int i = 0;

	/*** overflow & NULL checks ***/
	int overflow = 8 * sizeof(int);
	unsigned int length = strlen(s);

	if (length > overflow) {
		return -1;
	}
	if (length == 0) {
		return 0;
	}
	if (s == NULL) {
		return 0;
	}

	for (i = length - 1; i >= 0; i--) {
		if ((s[i] - '0') == 1 || (s[i]-'0') == 0) {
			/**** bit shift faster than pow! ****/
			dec = dec + (s[i] - '0') * (1 << x++);
		}
	}
	return dec;
}

int main(int argc, char *argv[]) {
	/*** Binary number is accepted through command line (argv[1]) ***/
	unsigned int decimal = bstr2int(argv[1]);
	printf("Your binary value %s, converted to decimal:\n%d\n", argv[1], decimal);

	return 0;
}
