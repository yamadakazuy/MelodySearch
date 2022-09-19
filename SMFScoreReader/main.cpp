#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "smf.h"

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::vector;

enum FUNCTION {
	SHOW_NOTES = 0,
	OUTPUT_MELODIC_CONTOUR,
};

int main(int argc, char **argv) {
	FUNCTION func = SHOW_NOTES;
	string filename;
	std::ifstream input;

	if ( !(argc > 1) ) {
		cerr << "[-contour] ファイル名" << endl;
		return EXIT_FAILURE;
	}

	for(int i = 1; i < argc; ++i) {
		if ( string(argv[i]) == "-contour" ) {
			func = OUTPUT_MELODIC_CONTOUR;
		} else {
			filename = argv[i];
		}
	}
	cout << "file: " << filename << endl;
	input.open(filename, (std::ios::in | std::ios::binary) );
	if ( !input ) {
		cerr << "オープン失敗" << endl;
		return EXIT_FAILURE;
	}
	smf::score midi(input);
	input.close();
	if ( ! midi.empty() ) {
		std::cerr << "SMF読み込み失敗" << std::endl;
		return EXIT_FAILURE;
	}
	//std::cout << midi << std::endl;

	if (func == SHOW_NOTES) {
		std::vector<smf::note> notes = midi.notes();
		std::cout << "notes size = " << notes.size() << std::endl << std::endl;
		for(auto i = notes.begin(); i != notes.end(); ++i) {
			std::cout << *i ;
			if ( i->time < (i+1)->time ) {
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
		std::cout << "SMPTE " << midi.isSMPTE() << " resolution = " << midi.resolution() << " format = " << midi.format() << std::endl;
	} else if (func == OUTPUT_MELODIC_CONTOUR) {
		std::ofstream outputfile;
	    filename += ".contour";
		outputfile.open(filename, std::ios::out);
		if ( !outputfile ) {
			cerr << "オープン失敗 " << filename << endl;
			return EXIT_FAILURE;
		}
		cout << "Writing melodic contour sequence to " << filename << "." << endl;
		//		    writing_file << writing_text << std::endl;
	   // writing_file.close();

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
							//std::cout << last_noteon << "\t";
							outputfile << ((notenum_2 == -1) ? "*" : (notenum_1 == notenum_2 ? "=" : (notenum_1 > notenum_2 ? "+" : "-")));
							//std::cout << notenum_1 << " <- " << notenum_2 ;
							//std::cout << std::endl;
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
				//std::cout << last_noteon << "\t";
				outputfile << ((notenum_2 == -1) ? "*" : (notenum_1 == notenum_2 ? "=" : (notenum_1 > notenum_2 ? "+" : "-")));
				//std::cout << notenum_1 << " <- " << notenum_2 ;
				//std::cout << std::endl;
				break;
			}
		}
		outputfile.close();
		cout << "done." << endl;
	}
	return 0;
}
