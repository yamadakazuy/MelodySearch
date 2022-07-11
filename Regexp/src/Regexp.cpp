//============================================================================
// Name        : Regexp.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <regex>
#include <filesystem>

using std::cout;
using std::endl;
using std::string;
namespace stdfs = std::filesystem;

int main(const int argc, const char * argv[]) {
	std::string path;
	if (argc > 1)
		path = argv[1];
	else
		exit(1);

	cout << "search for .txt in " << path << endl;

	std::regex rpatt(R"(Alice was)");
	std::match_results<string::const_iterator> res;

	unsigned int counter = 0;
	for (const stdfs::directory_entry & entry : stdfs::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if ( entry.path().string().ends_with(".txt") ) {
			counter += 1;
			cout << counter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			if ( std::regex_search(text, res, rpatt) ) {
				cout << res.str() << ", " << res.position() - res.length() << endl;
			} else {
				cout << "no match" << endl;
			}
		}
	}

	return 0;
}
