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
		ringtail = 0;
		ring[ringtail] = '\0';
	}

	~naive() {
		delete [] ring;
	}

	int length() { return patlen; }

	bool match(const char c) {
		int pos;
		ring[++ringtail] = c;
		printf("pos = ");
		for(pos = 0; pos < patlen; ++pos) {
			printf("%c %d-%d, ", pattern[pos], pos, (ringtail+1+pos) % patlen);
			if (pattern[pos] != ring[(ringtail+1+pos) % patlen])
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
			printf("matched at %d to %d\n", i-pmm.patlen+1, i);
		}
	}
	printf("finished.\n");

	return 0;
}
