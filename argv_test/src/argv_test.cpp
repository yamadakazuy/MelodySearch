//============================================================================
// Name        : argv_test.cpp
// Author      : Ayane
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

#define char2state(x)  (tolower(x) - 0x30)

void triple(int * x) {
	*x *=3;
}

int main(int argc, char ** argv) {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	printf("引数の個数は%dです。\n", argc);

	char *melody = argv[1];
	int len;

	len = strlen(melody);

	printf("最初の引数は%sです。\n", melody);
	printf("引数の文字の長さは%dです。\n", len);

	char triplex[72];

	sscanf(melody, "%s", triplex);
	for(char * x = triplex+2; *x; ++x) { 	/* 遷移先は複数記述可能 */
		cout << *x <<endl;
		cout << x <<endl;
		cout << char2state(*x) <<endl;
	}

	int symb = '+';
	cout << "symb:" << symb << endl;

//	int x = 3, y = -1;
//	triple(&x);
//	printf("x=%d, y=%d\n",x,y);

	return 0;
}
