#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

int strmatch(std::string text, std::string pattern, int start = 0) {
	int pos, ix;
	for(pos = start; pos < text.length(); ++pos) {
		for(ix = 0; ix < pattern.length()
		and pos + ix < text.length() and text[pos+ix] == pattern[ix]; ++ix) { }
		if ( ix == pattern.length() )
			return pos;
	}
	return -1;
}

int main(int argc, char **argv) {
	if (argc <= 2) {
		printf("C-style print to standard out by stdio.h\n");
		printf("usage: command pattern_string filename [return]\n");
		return 1;
	} else {
		std::cout << "C++-style print to standard out by iostream" << std::endl;
		std::cout << "pattern = \"" << argv[1] << "\", file name = " << argv[2] << std::endl;
	}

	std::string pattern(argv[1]);
	std::ifstream file;
	std::string line;

	file.open(argv[2], std::ios::in );
	if ( !file ) { // ! file.is_open() でも可
		std::cerr << "ファイルオープン失敗" << std::endl; // cerr 標準エラー出力
		return -1;
	}

	int pos;
	int counter = 0;
    while (getline(file, line)){
        pos = strmatch(line, pattern);
        if ( pos >= 0 ) {
        	std::cout << pattern << " matched at " << pos << " of line " << counter << std::endl;
        }
        counter += 1;
    }

	file.close();

	return 0;
}
