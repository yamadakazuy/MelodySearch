#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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

char contour(const int & noteprev, const int & notenum) {
	if ( noteprev == -1 ) {
		return '0';
	} else if ( notenum == noteprev ) {
		return '=';
	} else if ( notenum == noteprev + 1 ) {
		return '#';
	} else if ( notenum + 1 == noteprev ) {
		return 'b';
	} else if ( notenum > noteprev ) {
		return '+';
	} else if ( notenum < noteprev ) {
		return '-';
	} else {
		return '?';
	}
}

bool fileout_contour(const smf::score & midi, const string & filename) {
	vector<smf::note> noteseq[16];
	for(int i = 0; i < 16; ++i) {
		noteseq[i].clear();
	}

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

	std::ostringstream filenamess;
	for(int ch = 0; ch < 16; ++ch) {
		if ( ch == 9 or noteseq[ch].empty() ) {
			// skip the rhythm box channel and an empty channel.
			continue;
		}
		filenamess.str("");
		filenamess.clear();
		filenamess << filename << "_" << std::setw(2) << std::setfill('0') << std::dec << (ch+1) << ".cont";
		//cout << outfilenamess.str() << endl;
		std::ofstream out(filenamess.str(), std::ios::out);
		if ( ! out ) {
			cerr << filenamess.str() << " オープン失敗!!!" << endl;
			return false;
		}
		for(unsigned int i = 0; i < noteseq[ch].size(); ++i) {
			if ( i == 0 ) {
				out << "0";
			} else {
				out << contour(noteseq[ch][i-1].number, noteseq[ch][i].number);
			}
		}
		out.close();
	}
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
			std::filesystem::path outpath(entry.path());
			outpath.replace_extension("");
			cout << " melodic contour written to " << outpath.filename() << ".xx.cont";
			if ( fileout_contour(midi, outpath.string() ) ) {
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
