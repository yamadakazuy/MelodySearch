#include <iostream>
#include <cstdlib>
#include <stack>
#include <regex>
#include <dirent.h>

struct DirPathScanner {
	typedef std::pair<std::string,DIR*> pathdir;
	std::stack<pathdir> pathdirs;
	dirent * lastentry;
	std::regex pattern;
/*
#define DT_UNKNOWN       0
#define DT_FIFO          1
#define DT_CHR           2
#define DT_DIR           4
#define DT_BLK           6
#define DT_REG           8
#define DT_LNK          10
#define DT_SOCK         12
#define DT_WHT          14
*/

	DirPathScanner(const char * path, const char * pat = ".*") : pathdirs(), lastentry(NULL), pattern(pat) {
		DIR * dp = opendir(path);
		if ( dp == NULL ) {
			std::cerr << "error: opendir returned a NULL pointer for the path "
					<< path << std::endl;
			return;
		}
		pathdirs.push(pathdir(std::string(path),dp));
	}

	bool operator()() const {
		return !pathdirs.empty();
	}

	dirent * get_next_regular() {
		std::string subdirname;
		for(;;) {
			lastentry = readdir(pathdirs.top().second);
			if ( lastentry == NULL ) {
				pathdirs.pop();
				if ( pathdirs.empty() )
					break;
				continue;
			} else if ( is_directory() ) {
				//std::cout << pathdirs.top().first << " " << lastentry->d_name << std::endl;
				if ( strcmp(node_name(), ".") == 0 )
					continue;
				if ( strcmp(node_name(), "..") == 0 )
					continue;
				subdirname = pathdirs.top().first;
				if ( subdirname[subdirname.size()-1] != '/' ) {
					subdirname += "/";
				}
				subdirname += node_name();
				DIR * dp = opendir(subdirname.c_str());
				if ( dp == NULL ) {
					std::cerr << "error: opendir returned a NULL pointer for the path "
							<< subdirname << std::endl;
					break;
				} else {
					pathdirs.push(pathdir(subdirname, dp));
					continue;
				}
			} else if ( is_regular() ) {
				if ( std::regex_search(node_name(), pattern) ) {
					break;
				}
				continue;
			} else {
				//std::cout << "unknown d_type " << node_type() << std::endl;
				continue;
			}
		}
		return lastentry;
	}

	int node_type() const {
		if ( lastentry == NULL )
			return DT_UNKNOWN;
		return lastentry->d_type;
	}

	bool is_directory() const {
		return node_type() == DT_DIR;
	}

	bool is_regular() const {
		return node_type() == DT_REG;
	}

	const char * node_name() const {
		if ( lastentry == NULL )
			return (const char * ) NULL;
		return lastentry->d_name;
	}

	const char * node_path() const {
		if ( lastentry == NULL )
			return (const char * ) NULL;
		return pathdirs.top().first.c_str();
	}
};

int main(const int argc, const char *argv[]) {
	if (argc <= 1) {
		std::cerr << "error: give a base directory name." << std::endl;
		exit(1);
	}
	const char *basepath = argv[1];
	DirPathScanner dpscanner(basepath, ".*\\.mid");

	int counter = 0;
	if ( ! dpscanner() ) {
		std::cerr << "error: opendir returned a NULL pointer." << std::endl;
		exit(1);
	}
	while (dpscanner.get_next_regular() != NULL) {
		++counter;
		std::cout << counter << ": " << dpscanner.node_path() << "/" << dpscanner.node_name() << std::endl;
	}
	return 0;
}
