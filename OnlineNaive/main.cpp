#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct naive {
	const char * pattern;
	const int patlen;
	char * ring;
	int ringtail;

	naive(const char * pat) : pattern(pat), patlen(strlen(pat)) {
		ring = new char[patlen];
		ring[patlen-1] = '\0';
		ringtail = 0;
	}

	~naive() {
		delete [] ring;
	}

	int length() { return patlen; }

	void append_to_ring(const char c) {
		ring[ringtail] = c;
		ringtail = (ringtail+1)%patlen;
		for(int i = 0; i< patlen; ++i) {
			printf("%c", ring[(ringtail+i)%patlen]);
		}
	}

	bool match(const char c) {
		int pos;
		append_to_ring(c);
		for(pos = 0; pos < patlen; ++pos) {
			printf(": ");
			printf("%c %d-%d %c, ", pattern[pos], pos, (ringtail+pos) % patlen, ring[(ringtail+pos) % patlen]);
			if (pattern[pos] != ring[(ringtail+pos) % patlen])
				break;
		}
		printf("\n");
		return pos == patlen;
	}
};

int main(int argc, char * argv[]) {
	char t[] = "mississippi river";
	char p[] = "issi";

	printf("text = %s,\npattern = %s\n", t, p);

	naive pmm(p);

	printf("patlen = %d\n", pmm.length());
	for(int i = 0; i < strlen(t); ++i) {
		if (pmm.match(t[i])) {
			printf("matched at [%d,%d]\n", i-pmm.patlen+1, i);
		}
	}
	printf("finished.\n");

	return 0;
}
