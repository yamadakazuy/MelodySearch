#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

//C++17
#include <filesystem>

//C++11/C++20
#include <chrono>

#include "smf.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;

char contour(const int & diff) {
	switch(diff) {
	case 0:
		return '=';
	case 1:
		return '#';
	case -1:
		return 'b';
	}
	if ( diff > 1 ) {
		return '+';
	} else if ( diff < -1 ) {
		return '-';
	}
	return '0'; //　This should never happen.
}

bool fileout_score(const smf::score & midi, const string & filename) {
	// read smf::score as noteseq
	//cerr << endl << "fileout_score " << filename << endl;
	vector<smf::note> noteseq[16];
	for(int i = 0; i < 16; ++i) {
		noteseq[i].clear();
	}

	cout << " converting.." << std::flush;
	for(const auto & a_note : midi.notes() ) {
		int nn = int(a_note.number);
		int ch = int(a_note.channel);
		unsigned int tm = int(a_note.time);
		if ( noteseq[ch].empty() or noteseq[ch].back().time != tm ) {
			noteseq[ch].push_back(a_note);
		} else { // if not empty and the last note is in the same voice
			if ( noteseq[ch].back().number < nn ) {
				noteseq[ch].pop_back();
				noteseq[ch].push_back(a_note);
			}
		}
	}

	cout << ". " << std::flush;
	// fileout noteseq
	std::ofstream out(filename, std::ios::out);
	if ( ! out ) {
		cerr << filename << " オープン失敗!!!" << endl;
		return false;
	}
	for(int ch = 0; ch < 16; ++ch) {
		if ( ch == 9 or noteseq[ch].empty() ) {
			// skip the rhythm box channel and empty channels.
			continue;
		}
		out << int(ch) << ",";
		for(unsigned int i = 1; i < noteseq[ch].size(); ++i) {
			out << contour(noteseq[ch][i].number - noteseq[ch][i-1].number);
		}
		out << endl;
	}
	out.close();

	return true;
}

int main(int argc, char **argv) {
	std::ifstream input;

	cout << "SMF2Contour Converter ver. 0.9.0" << endl;

	if ( !(argc > 1) ) {
		cerr << "usage: SMF2cont dirname [enter]" << endl;
		return EXIT_FAILURE;
	}
	std::filesystem::path argpath(argv[1]);
	if ( !std::filesystem::exists(argpath) ) {
		cerr << "Directory or file " << argpath.string() << " does not exist." << endl;
		return EXIT_FAILURE;
	}
	if ( !std::filesystem::is_directory(argpath) ) {
		cerr << "Path is not a directory." << endl;
		return EXIT_FAILURE;
	}

	auto start = std::chrono::system_clock::now(); // 計測開始時刻

	string pathext;
	for (const auto & entry : std::filesystem::recursive_directory_iterator(argpath)) {
		if (entry.is_directory())
			continue;
		pathext = entry.path().extension().string();
		if ( pathext.ends_with(".mid") or pathext.ends_with(".MID") ) {
		 // ends_with() --- introduced in C++20
			cout << "File " << entry.path().filename();
			input.open(entry.path().string(), (std::ios::in | std::ios::binary) );
			if ( !input ) {
				cerr << " open failed." << endl;
				continue;
			}

			smf::score midi(input);
			input.close();
			if ( midi.is_empty() ) {
				std::cerr << "Reading SMF failed. Skip." << std::endl;
				continue;
			}
			cout << midi << endl;
			std::filesystem::path outpath(entry.path());
			outpath.replace_extension(".cont");
			cout << " writing melodic contour into " << outpath.string();
			if ( fileout_score(midi, outpath.string()) ) {
				cout << ", done." << endl;
			} else {
				cout << " conversion failed." << endl;
			}
		}
	}

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return EXIT_SUCCESS;
}
