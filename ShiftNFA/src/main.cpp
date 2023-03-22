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
#include "MyNaive.h"

int main(int argc, char **argv) {

	string path = "", melody = "", text = "default";
	int argstrc = 0;

	enum {
		MODE_SILENT = 0,
		MODE_MODERATE = 1,
		MODE_VERBOSE = 2,
		MODE_TEST = 3,
	} verbose_mode = MODE_MODERATE; // 2 で印字出力多め

	enum {
		PM_NAIVE = 1,
		PM_MYNFA = 2,
		PM_SHIFTNFA = 4,
	} pm = PM_MYNFA;

	bool test_mode = false;
	bool show_pm = false;
	if (argc >= 3) {
		for (int i = 1 ; i < argc; ++i) {
			//cout << argv[i] << endl;
			if ( string(argv[i]).starts_with('-') ) {
				if (string("-v") == string(argv[i]) ) {
					verbose_mode = MODE_VERBOSE;
					continue;
				} else if (string("-s") == string(argv[i]) ) {
					verbose_mode = MODE_SILENT;
					continue;
				} else if (string("-naive") == string(argv[i]) ) {
					pm = PM_NAIVE;
					continue;
				} else if (string("-my") == string(argv[i]) ) {
					pm = PM_MYNFA;
					continue;
				} else if (string("-shift") == string(argv[i]) ) {
					pm = PM_SHIFTNFA;
					continue;
				} else if (string("-show") == string(argv[i]) ) {
					show_pm = true;
					continue;
				} else if (string("-test") == string(argv[i]) ) {
					test_mode = true;
					continue;
				}
			}
			//cout << argstrc << ": " << argv[i] << endl;
			if ( argstrc == 0 ) {
				path = argv[i];
			} else if ( argstrc == 1 ) {
				melody = argv[i];
			} else if (argstrc == 2 ) {
				cout << "text" << endl;
				text = argv[i];
			}
			++argstrc;
		}
	}

	if ( path.length() == 0 and melody.length() == 0 ) {
		cout << "requires dirpath melody" << endl;
		exit(1);
	}

	MyNFA mmy(melody);
	ShiftNFA mshift(melody);
	NaiveSearcher naive(melody);

	if ( show_pm ) {
		cout << "search " << melody << " for .cont in " << path << "." << endl;
		if ( pm == PM_NAIVE )
			cout << "Naive = " << naive << endl;
		if ( pm == PM_MYNFA )
			cout << "NFA = " << mmy << endl;
		if ( pm == PM_SHIFTNFA )
			cout << "ShiftNFA = " << mshift << endl;
	}

	if ( test_mode ) {
		cout << "text = " << text << endl;
		if ( pm == PM_NAIVE )
			cout << "Naive " << naive.run(text.c_str()) << endl;
		if ( pm == PM_MYNFA )
			cout << "MyNFA " << mmy.run(text.c_str()) << endl;
		if ( pm == PM_SHIFTNFA )
			cout << "ShiftNFA " << mshift.run(text.c_str()) << endl;
		exit(0);
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
			auto start_search = chrono::system_clock::now(); // 計測開始時刻
			long pos = -1;
			string chnum, track;
			while(std::getline(ifs, chnum, ',') ) {
				std::getline(ifs, track);
				cout << " track length = " << track.length() << endl;
				bytecounter += track.length();
				if ( pm == PM_NAIVE ) {
					pos = naive.run(track);
				} else if ( pm == PM_MYNFA ) {
					pos = mmy.run(track);
				} else if ( pm == PM_SHIFTNFA ) {
					pos = mshift.run(track);
				}
				if ( pos >= 0 ){
					if ( verbose_mode != MODE_SILENT ) {
						cout << filecounter << " " << entry.path().string();
						cout << " match at ch. " << chnum << ", " << pos << "." << endl;
					}
					hitcounter++;
					break;
				}
				/*
				else {
					if ( verbose_mode == MODE_VERBOSE ) {
						cout << filecounter << " " << entry.path().string();
						cout << "no match." << endl;
					}
				}
				*/
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
