//============================================================================
// Name        : bitmanip64.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <iomanip>

#include <cinttypes>
using namespace std;

int main() {
	cout << "!!!Welcome to Bit-Manipulation World!!!" << endl; // prints !!!Hello World!!!

	cout << endl << setfill(' ') << setw(20) << "int type name" << "\t" << "bits" << endl;
	cout << setfill(' ') << setw(20) << "------------" << "\t" << "-----" << endl;
	cout << setfill(' ') << setw(20) << "unsigned char" << "\t" << sizeof(unsigned char)*8 << endl;
	cout << setfill(' ') << setw(20) << "unsigned short" << "\t" << sizeof(unsigned short)*8 << endl;
	cout << setfill(' ') << setw(20) << "unsigned int" << "\t" << sizeof(unsigned int)*8 << endl;
	cout << setfill(' ') << setw(20) << "unsigned long" << "\t" << sizeof(unsigned long)*8 << endl;
	cout << setfill(' ') << setw(20) << "unsigned long long" << "\t" << sizeof(unsigned long long)*8 << endl;
	cout << setfill(' ') << setw(20) << "------------" << "\t" << "-----" << endl;
	cout << setfill(' ') << setw(20) << "uint64_t" << "\t" << sizeof(uint64_t)*8 << endl;
	cout << setfill(' ') << setw(20) << "uintmax_t" << "\t" << sizeof(uintmax_t)*8 << endl;

	uint64_t val = 0x040133085401a00fLL;
	cout << setfill('0') << setw(16) << hex << val << "\t" << __builtin_clzll(val) << endl;
	cout << setfill('0') << setw(16) << hex << val << "\t" << __builtin_clzll(val)+1 << endl;
	val = 0;
	cout << setfill('0') << setw(16) << hex << val << "\t" << __builtin_clzll(val) << endl;

	return 0;
}
