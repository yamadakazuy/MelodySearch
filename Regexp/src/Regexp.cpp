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
#include <regex>
// requires C++20
#include <filesystem>

//C++11/C++20
#include <chrono>


using std::cout;
using std::endl;
using std::string;
namespace fsys = std::filesystem;

int main(const int argc, const char * argv[]) {
	string path, regexpstr;
	int hit = 0;

	if (argc >= 3) {
		path = argv[1];
		regexpstr = argv[2];
	} else {
		cout << "requires path regexp " << endl;
		exit(1);
	}
	//cout << "search " << regexpstr << " for .contour in " << path << endl;

	std::regex rpatt(regexpstr); //rpatt(R"(Alice was)");
	std::match_results<string::const_iterator> res;

	unsigned int counter = 0;
	auto start = std::chrono::system_clock::now(); // 計測開始時刻

	for (const fsys::directory_entry & entry : fsys::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if ( entry.path().string().ends_with(".cont") ) {
			counter += 1;
			cout << counter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			if ( std::regex_search(text, res, rpatt) ) {
//				cout << res.str() << ", " << res.position() << endl;
				cout << "match , " << res.position() + res.length() - 1 << endl;
				hit++;
			} else {
				cout << "no match" << endl;
			}
		}
	}

	cout << "hit = " << hit << endl;

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
