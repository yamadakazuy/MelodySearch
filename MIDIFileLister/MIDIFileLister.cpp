#include <iostream>
#include <cstdlib>
#include <stack>
#include <dirent.h>

struct DirPathScanner {
	typedef std::pair<std::string,DIR*> pathdir;
	std::stack<pathdir> pathdirs;
	dirent * lastentry;


	DirPathScanner(const char * path) : pathdirs(), lastentry(NULL) {
		DIR * dp = opendir(path);
		if ( dp == NULL ) {
			std::cerr << "error: opendir returned a NULL pointer for the path "
					<< path << std::endl;
			return;
		}
		pathdirs.push(pathdir(std::string(path),dp));
	}

	bool operator()() const {
		return pathdirs.empty();
	}

	dirent * next_entry() {
		for(;;) {
			lastentry = readdir(pathdirs.top().second);
			if ( lastentry == NULL ) {
				pathdirs.pop();
				if ( pathdirs.empty() )
					break;
				continue;
			} else if ( lastentry->d_type == DT_DIR ) {
				if ( strcmp(lastentry->d_name, ".") == 0 )
					continue;
				if ( strcmp(lastentry->d_name, "..") == 0 )
					continue;
				std::string subdirname = pathdirs.top().first + lastentry->d_name;
				DIR * dp = opendir(subdirname.c_str());
				if ( dp == NULL ) {
					std::cerr << "error: opendir returned a NULL pointer for the path "
							<< subdirname << std::endl;
				} else {
					pathdirs.push(pathdir(subdirname, dp));
				}
			} else if ( lastentry->d_type == DT_REG ) {
				if ( strncmp(lastentry->d_name, ".", 1) == 0 )
					continue;
				std::cout << "file, " << "base path: " << basepath << ", name: " << entry->d_name << std::endl;
			} else {
				std::cout << "unknown d_type " << (int) entry->d_type << std::endl;
			}
		}
		return lastentry;
	}
};

int main(const int argc, const char *argv[]) {
	if (argc <= 1) {
		std::cerr << "error: give a base directory name." << std::endl;
		exit(1);
	}
	const char *basepath = argv[1];
	DirPathScanner dps(basepath);
//	DIR *dp;       // ディレクトリへのポインタ
//	dirent *entry; // readdir() で返されるエントリーポイント

//	dp = opendir(basepath);
	if ( ! dps() ) {
		std::cerr << "error: opendir returned a NULL pointer." << std::endl;
		exit(1);
	}
	while (true) {

		if (dps.next_entry() == NULL)
			break;
		if ( entry->d_type == DT_DIR ) {
			if ( strcmp(entry->d_name, ".") == 0 )
				continue;
			if ( strcmp(entry->d_name, "..") == 0 )
				continue;
			std::cout << "dir, " << "base path: " << basepath << ", name: " << entry->d_name << std::endl;
		} else if ( entry->d_type == DT_REG ) {
			if ( strncmp(entry->d_name, ".", 1) == 0 )
				continue;
			std::cout << "file, " << "base path: " << basepath << ", name: " << entry->d_name << std::endl;
		} else {
			std::cout << "unknown d_type " << (int) entry->d_type << std::endl;
		}
	}
	return 0;
}
