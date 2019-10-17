#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]) {
	int * f;
	char t[] = "mississippi river";
	char p[] = "ssissi";

	printf("text = %s,\npattern = %s\n", t, p);

	f = (int*) malloc(sizeof(int)*strlen(p)); // in C fashion
	// f = new int[strlen(p)]; // in C++ fashion

	for(int skip = 1; skip < strlen(p); ++skip) {
		int i;
		for(i = 0; p[i] == p[skip+i] && p[skip+i] != 0; ++i);
		f[skip] = (i>1? i : 1);
	}
	for(int i = 0; i < strlen(p); ++i) {
		printf("f[%d] = %d,\n", i, f[i]);
	}
	free(f); // in C fashion
	// delete [] f; // in C++ fashion
	return 0;
}
