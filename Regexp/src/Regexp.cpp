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

	if (argc >= 3) {
		path = argv[1];
		regexpstr = argv[2];
	} else {
		cout << "requires path regex " << endl;
		exit(1);
	}
	//cout << "search " << regexpstr << " for .contour in " << path << endl;

	std::regex rpatt(regexpstr); //rpatt(R"(Alice was)");
	std::match_results<string::const_iterator> res;

	unsigned int hitcounter = 0, filecounter = 0;
	unsigned long search_micros = 0, total_millis = 0;

	auto start_total = std::chrono::system_clock::now(); // 計測開始時刻
	for (const fsys::directory_entry & entry : fsys::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if ( entry.path().string().ends_with(".cont") ) {
			filecounter ++;
			cout << filecounter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			auto start_search = std::chrono::system_clock::now(); // 計測開始時刻
			bool matched = std::regex_search(text, res, rpatt);
			auto stop_search = std::chrono::system_clock::now(); 	// 計測終了時刻
			search_micros += std::chrono::duration_cast<std::chrono::microseconds >(stop_search - start_search).count(); // ミリ秒に変換
			if ( matched ) {
//				cout << res.str() << ", " << res.position() << endl;
				cout << "match , " << res.position() + res.length() - 1 << endl;
				hitcounter++;
			} else {
				cout << "no match" << endl;
			}
		}
	}
	auto stop_total = std::chrono::system_clock::now(); 	// 計測終了時刻
	total_millis += std::chrono::duration_cast<std::chrono::milliseconds >(stop_total - start_total).count(); // ミリ秒に変換

	cout << "hits = " << hitcounter << endl;
	cout << "It took " << search_micros << " micros in search, totaly "<< total_millis << " milli seconds." << endl;

	return 0;
}
