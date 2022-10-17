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

int main(int argc, char **argv) {
	string filename;
	std::ifstream input;
	bool parse_only = false;

	if (argc <= 1) {
		cerr << "Give an argument as a file name." << endl;
		return EXIT_FAILURE;
	} else {
		int aix = 1;
		while (aix < argc) {
			if ( argv[aix][0] == '-' ) {
				// -option
				if ( string(argv[aix]) == "-parse" ) {
					parse_only = true;
				}
			} else {
				filename = argv[aix];
			}
			++aix;
		}
		if (filename.length() == 0) {
			cerr << "No file name is given." << endl;
			return EXIT_FAILURE;
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
	/*
	for(int i = 0; i < midi.noftracks(); ++i) {
		for(auto evitr = midi.track(i).begin(); evitr != midi.track(i).end(); ++evitr) {
			cout << *evitr << endl;
		}
	}
	*/
	if ( !parse_only ) {
		std::vector<smf::note> notes = midi.notes();
		std::cout << "notes size = " << notes.size() << std::endl << std::endl;
		for(auto i = notes.begin(); i != notes.end(); ++i) {
			std::cout << *i ;
			if ( i->time < (i+1)->time ) {
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
	}

	midi.header_info(std::cout);
	std::cout << " SMPTE " << midi.isSMPTE() << " resolution = " << midi.resolution() << " format = " << midi.format() << std::endl;

	return EXIT_SUCCESS;
}
