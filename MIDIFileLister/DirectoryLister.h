#ifndef _DIRECTORYLISTER_H_
#define _DIRECTORYLISTER_H_

#include <iostream>
#include <cstdlib>
#include <deque>
#include <regex>
#include <dirent.h>

struct DirectoryLister {
	typedef std::pair<std::string,DIR*> pathdir;
	std::deque<pathdir> pathdirs;
	dirent * lastentry;

#define A_SUBDIR _A_SUBDIR
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

	DirectoryLister(const char *path) : pathdirs(), lastentry(NULL) {
		std::string rootpath(path);
		if ( rootpath.size() != 0 and rootpath[rootpath.size()-1] == '/' )
			rootpath.pop_back();
		DIR *dp = opendir(rootpath.c_str());
		if (dp == NULL) {
			std::cerr << "error: opendir returned a NULL pointer for the path "
					<< path << std::endl;
			return;
		}
		pathdirs.push_back(pathdir(rootpath, dp));
	}

	bool operator()() const {
		return !pathdirs.empty();
	}

	const std::string & rootpath() const {
		return pathdirs.front().first;
	}

	dirent * get_next_file(const char * regpat = ".*") {
		std::regex fpattern(regpat);
		std::string subdir;
		for(;;) {
			lastentry = readdir(pathdirs.back().second);
			if ( lastentry == NULL ) {
				pathdirs.pop_back();
				if ( pathdirs.empty() )
					break;
				continue;
			} else if ( entry_isdir() ) {
				//std::cout << pathdirs.top().first << " " << lastentry->d_name << std::endl;
				if ( strcmp(entry_name(), ".") == 0 )
					continue;
				if ( strcmp(entry_name(), "..") == 0 )
					continue;
				subdir = "/";
				subdir += entry_name();
				if ( pathdirs.back() != pathdirs.front() )
					subdir = pathdirs.back().first + subdir;
				DIR * dp = opendir((rootpath()+subdir).c_str());
				//std::cerr << (rootpath()+subdir) << std::endl;
				if ( dp == NULL ) {
					std::cerr << "error: opendir returned a NULL pointer for the path "
							<< subdir << std::endl;
					break;
				} else {
					pathdirs.push_back(pathdir(subdir, dp));
					continue;
				}
			} else if ( entry_isreg() ) {
				if ( std::regex_match(entry_name(), fpattern) ) {
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

	int entry_type() const {
		if ( lastentry == NULL )
			return DT_UNKNOWN;
		return lastentry->d_type;
	}

	bool entry_isdir() const {
		return entry_type() == DT_DIR;
	}

	bool entry_isreg() const {
		return entry_type() == DT_REG;
	}

	const char * entry_name() const {
		if ( lastentry == NULL )
			return (const char * ) NULL;
		return lastentry->d_name;
	}

	const char * entry_path() const {
		if ( lastentry == NULL )
			return (const char * ) NULL;
		if (pathdirs.front() == pathdirs.back())
			return "";
		return pathdirs.back().first.c_str();
	}

	const char * entry_fullpath() const {
		if ( lastentry == NULL )
			return (const char * ) NULL;
		return (pathdirs.front().first + pathdirs.back().first).c_str();
	}
};

#endif
