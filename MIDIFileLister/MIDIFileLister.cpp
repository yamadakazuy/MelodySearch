#include <iostream>
#include <cstdlib>
#include "DirectoryLister.h"

int main(const int argc, const char *argv[]) {
	if (argc <= 1) {
		std::cerr << "error: give a base directory name." << std::endl;
		exit(1);
	}
	const char *path = argv[1];
	const char dontcare[] = ".*";
	const char * p;
	if ( argc == 3 ) {
		p = argv[2];
	} else {
		p = dontcare;
	}

	DirectoryLister dlister(path);

	int counter = 0;
	if ( ! dlister() ) {
		std::cerr << "error: opendir returned a NULL pointer for the base path." << std::endl;
		exit(1);
	}
	while (dlister.get_next_file(p) != NULL) {
		++counter;
		std::cout << counter << ": " << dlister.entry_path().c_str() << std::endl;
	}
	std::cout << std::endl << "finished." << std::endl << std::endl;
	return 0;
}