//============================================================================
// Name        : ShiftNFA.cpp
// Author      : Sin Shimozono
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <cinttypes>

// requires C++20
#include <filesystem>

using namespace std;

#include "bset64.h"
#include "MyNFA2.h"
#include "ShiftNFA.h"

int main(int argc, char **argv) {

	string path = "", melody;

	enum {
		SILENT_MODE = 0,
		MODERATE_MODE = 1,
		VERBOSE_MODE = 2,
	} verbose_mode = MODERATE_MODE; // 2 で印字出力多め

	if (argc >= 3) {
		for (int i = 1; i < argc; ++i) {
			if (string("-v") == string(argv[i]) ) {
				verbose_mode = VERBOSE_MODE;
			} else if (string("-s") == string(argv[i]) ) {
				verbose_mode = SILENT_MODE;
			} else {
				if (path.length() == 0)
					path = argv[i];
				else
					melody = argv[i];
			}
		}
	} else {
		cout << "requires dirpath melody" << endl;
		exit(1);
	}

	ShiftNFA m(melody);

	long filecounter = 0;
	long hitcounter = 0;
	long bytecounter = 0;
	long long search_micros = 0, total_millis = 0;

	auto start_total = std::chrono::system_clock::now(); // 計測開始時刻
	for (const filesystem::directory_entry &entry :
			filesystem::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if (entry.path().string().ends_with(".cont")) {
			filecounter ++;
			ifstream ifs(entry.path().string());
			string text((istreambuf_iterator<char>(ifs)),
					istreambuf_iterator<char>());
			//char* input= &*text.begin();
			bytecounter += text.length();
			auto start_search = chrono::system_clock::now(); // 計測開始時刻
			long long pos = m.run(text.c_str());
			if ( pos >= 0 ){
				if ( verbose_mode != SILENT_MODE ) {
					cout << filecounter << " " << entry.path().string();
					cout << " match at " << pos << "." << endl;
				}
				hitcounter++;
			} else {
				if ( verbose_mode == VERBOSE_MODE ) {
					cout << filecounter << " " << entry.path().string() << endl;
					cout << "no match." << endl;
				}
			}
			auto stop_search = chrono::system_clock::now(); 	// 計測終了時刻
			search_micros += chrono::duration_cast<std::chrono::microseconds >(stop_search - start_search).count(); // ミリ秒に変換
		}
	}
	auto stop_total = chrono::system_clock::now(); 	// 計測終了時刻
	total_millis += chrono::duration_cast<chrono::milliseconds >(stop_total - start_total).count(); // ミリ秒に変換

	if (verbose_mode == VERBOSE_MODE ) {
		cout << "NFA = " << m << endl;
		cout << "search " << melody << " for .cont in " << path << "." << endl;
	}
	cout << "hits = " << hitcounter << " among " << filecounter << " files " << bytecounter << " bytes." << endl;
	cout << "It took " << search_micros << " micros in search, totaly "<< total_millis << " milli seconds." << endl;

	return 0;
}
