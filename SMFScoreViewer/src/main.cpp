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

	if (argc <= 1) {
		cerr << "Give an argument as a file name." << endl;
		return EXIT_FAILURE;
	}
	filename = argv[1];

	cout << "file: " << filename << endl;
	input.open(filename, (std::ios::in | std::ios::binary) );
	if ( !input ) {
		cerr << "オープン失敗" << endl;
		return EXIT_FAILURE;
	}
	smf::score midi(input);

	//cout << "midi" << endl;
	input.close();
	if ( ! midi.empty() ) {
		std::cerr << "SMF読み込み失敗" << std::endl;
		return EXIT_FAILURE;
	}
	//std::cout << midi << std::endl;

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

	return 0;
}
