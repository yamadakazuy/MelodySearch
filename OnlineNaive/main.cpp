#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct naive {
	const char * pattern;
	const int patlen;
	int matchpos;
	char * ring;
	int ringpos;

	naive(const char * patt) : pattern(patt), patlen(strlen(patt)) {
		matchpos = 0;
		ring = new char[patlen];
		ringpos = 0;
	}

	~naive() {
		delete [] ring;
	}

	bool match(const char c) {
		if ( c == '\0' )
			return false;
		ring[ringpos] = c;
		ringpos += 1;
		ringpos %= patlen;
		int pos, rpos;
		for(pos = 0, rpos = (ringpos+1)% patlen;
				pos < patlen;
				++pos, rpos = (rpos+1)% patlen) {
			if (pattern[pos] != ring[rpos])
				break;
		}
		if (pos == patlen)
			return true;
		return false;
	}
};

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
