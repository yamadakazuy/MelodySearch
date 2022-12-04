//============================================================================
// Name        : Charmapper.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char * argv[]) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	char * input = (char*)"b#*+-=";
	if ( argc > 1 )
		input = argv[1];
	//constexpr int charmap[]

	cout << input << endl;
	for(int i = 0; i < strlen(input); ++i) {
		int ch = input[i];
		cout << hex << (ch & 0x0f);
	}
	cout << endl;
	return 0;
}
