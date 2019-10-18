#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct kmp {
	const char * pattern;
	const int patlen;
	int * failure;

	kmp(const char * pat) : pattern(pat), patlen(strlen(pat)) {
		failure = new int[patlen];
		for(int i=0; i < patlen; ++i)
			failure[i] = 0;
		init_failure();
	}

	~kmp() {
		delete [] failure;
	}

	void init_failure();

	bool match(const char c) {
		return false;
	}
};

void kmp::init_failure() {
	int skip, i;
	for(skip = 1; skip < patlen; ++skip) {
		for(i = 0; && pattern[skip+i] == pattern[i]; ++i);

	}
}

int main(int argc, char * argv[]) {
	char t[] = "mississippi river";
	char p[] = "ssissi";

	printf("text = %s,\npattern = %s\n", t, p);

	naive pmm(p);

	for(int i = 0; i < strlen(t); ++i) {
		if (pmm.match(t[i])) {
			printf("matched at %d to %d\n", i-pmm.patlen+1, i);
		}
	}
	printf("finished.\n");

	return 0;
}
