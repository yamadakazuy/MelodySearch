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

using std::cout;
using std::cerr;
using std::endl;
using namespace std;

#include "bset64.h"
#include "MyNFA.h"
#include "ShiftNFA.h"

int main(int argc, char **argv) {

	string path = "", melody;

	enum {
		MODE_SILENT = 0,
		MODE_MODERATE = 1,
		MODE_VERBOSE = 2,
	} verbose_mode = MODE_MODERATE; // 2 で印字出力多め

	enum {
		PM_MYNFA = 0,
		PM_SHIFTNFA = 1,
	} pm = PM_MYNFA; // 2 で印字出力多め

	bool show_pm = false;
	if (argc >= 3) {
		for (int i = 1; i < argc; ++i) {
			if ( string(argv[i]).starts_with('-') ) {
				if (string("-v") == string(argv[i]) ) {
					verbose_mode = MODE_VERBOSE;
				} else if (string("-s") == string(argv[i]) ) {
					verbose_mode = MODE_SILENT;
				} else if (string("-my") == string(argv[i]) ) {
					pm = PM_MYNFA;
				} else if (string("-shift") == string(argv[i]) ) {
					pm = PM_SHIFTNFA;
				} else if (string("-show") == string(argv[i]) ) {
					show_pm = true;
				}
			} else {
				if (path.length() == 0)
					path = argv[i];
				else
					melody = argv[i];
			}
		}
	}

	if ( path.length() == 0 and melody.length() == 0 ) {
		cout << "requires dirpath melody" << endl;
		exit(1);
	}

	MyNFA mmy(melody);
	ShiftNFA mshift(melody);

	if ( show_pm ) {
		cout << "search " << melody << " for .cont in " << path << "." << endl;
		cout << "NFA = " << mmy << endl;
		cout << "ShiftNFA = " << mshift << endl;
	}

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
			long pos;
			if ( pm == PM_MYNFA ) {
				pos = mmy.run(text.c_str());
			} else if ( pm == PM_SHIFTNFA ) {
				pos = mshift.run(text.c_str());
			}
			if ( pos >= 0 ){
				if ( verbose_mode != MODE_SILENT ) {
					cout << filecounter << " " << entry.path().string();
					cout << " match at " << pos << "." << endl;
				}
				hitcounter++;
			} else {
				if ( verbose_mode == MODE_VERBOSE ) {
					cout << filecounter << " " << entry.path().string();
					cout << "no match." << endl;
				}
			}
			auto stop_search = chrono::system_clock::now(); 	// 計測終了時刻
			search_micros += chrono::duration_cast<std::chrono::microseconds >(stop_search - start_search).count(); // ミリ秒に変換
		}
		//break; ///////////////////////////////////////////////////////////
	}
	auto stop_total = chrono::system_clock::now(); 	// 計測終了時刻
	total_millis += chrono::duration_cast<chrono::milliseconds >(stop_total - start_total).count(); // ミリ秒に変換

	cout << "hits = " << hitcounter << " among " << filecounter << " files " << bytecounter << " bytes." << endl;
	cout << "It took " << search_micros << " micros in search, totaly "<< total_millis << " milli seconds." << endl;

	return 0;
}
