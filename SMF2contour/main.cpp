#include <iostream>
#include <fstream>
#include <string>
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

const string contour(const int & noteprev, const int & notenum) {
	if ( noteprev == -1 ) {
		return "0";
	} else if ( notenum == noteprev ) {
		return "=";
	} else if ( notenum == noteprev + 1 ) {
		return "#";
	} else if ( notenum + 1 == noteprev ) {
		return "b";
	} else if ( notenum > noteprev ) {
		return "+";
	} else if ( notenum < noteprev ) {
		return "-";
	} else {
		return "?";
	}
}

bool fileout_contour(const smf::score & midi, const string & filename) {
	std::ofstream out(filename, std::ios::out);
	if ( ! out ) {
		cerr << filename << " オープン失敗!!!" << endl;
		return false;
	}
	int64_t globaltime, last_noteon;
	int notenum, notenum_1, notenum_2, note_count;
	for(int i = 0; i < midi.noftracks(); ++i ) {
		globaltime = 0, last_noteon = -1;
		notenum = -1;
		note_count = 0;
		for(const auto & evt : midi.track(i)) {
			if (evt.deltaTime() > 0)
				globaltime += evt.deltaTime();
			if ( evt.isNoteOn() ) {
				++note_count;
				if (last_noteon < globaltime) {
					// new voice
					notenum_2 = notenum_1;
					notenum_1 = notenum;
					notenum = evt.notenumber();
					if (notenum_1 != -1) {
						out << contour(notenum_2, notenum_1);
					}
					last_noteon = globaltime;
				} else {
					notenum = std::max(notenum, evt.notenumber());
				}
			}
		}
		if (note_count > 0) {
			if (notenum_1 == -1)
				notenum_1 = notenum;
			out << contour(notenum_2, notenum_1);
			break;
		}
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

	for (const auto & entry : std::filesystem::recursive_directory_iterator(argpath)) {
		if (entry.is_directory())
			continue;
		if ( ! (entry.path().extension() == ".mid") )
			continue;
	 // ends_with() --- introduced in C++20
		cout << "File " << entry.path().filename();
		input.open(entry.path().string(), (std::ios::in | std::ios::binary) );
		if ( !input ) {
			cerr << " open failed." << endl;
			continue;
		}

		smf::score midi(input);
		input.close();
		if ( ! midi.empty() ) {
			std::cerr << "Reading SMF failed. Skip." << std::endl;
			continue;
		}
		std::filesystem::path outpath(entry.path());
		outpath.replace_extension(".cont");
		cout << " melodic contour written to " << outpath.filename();
		if ( fileout_contour(midi, outpath.string() ) ) {
			cout << ", done." << endl;
		} else {
			cout << " conversion failed." << endl;
		}
	}

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return EXIT_SUCCESS;
}
