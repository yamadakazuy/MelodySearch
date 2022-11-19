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

#include <random>

using namespace std;

// population count
inline int popc64(const uint64_t & val) { return __builtin_popcountll(val); }
// count the number of leading 0's
inline int clz64(const uint64_t & val) { return __builtin_clzll(val); }

// count the number of trailing 0's
inline int ctz64(const uint64_t & val) { return __builtin_ctzll(val); }

// clear the least significant bit
inline void clrlsb64(uint64_t & val) { val &= (val - 1); }

const char * print64bin(const uint64_t & val) {
	static char buf[65];
	uint64_t bmask = 1LL << 63;
	for(int i = 0; i < 64; ++i) {
		buf[i] = '0' + ((val & bmask) != 0);
		bmask >>= 1;
	}
	buf[64] = (char) 0;
	return buf;
}

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
	cout << setfill('0') << setw(16) << hex << val << "\t" << clz64(val) << endl;
	cout << setfill('0') << setw(16) << hex << val << "\t" << clz64(val)+1 << endl;
	val = 0;
	cout << setfill('0') << setw(16) << hex << val << "\t" << clz64(val) << endl;
	cout << endl;

	std::random_device dev;
    std::mt19937_64 rg(dev());

    for(int i = 0; i < 10; ++i) {
    	uint64_t t = rg() & rg();
    	cout << "0b" << print64bin(t) << " popc64 = " << dec << popc64(t) << endl;
    	int popcnt;
    	for ( popcnt = 0; t ; ++popcnt ) {
    		int bpos = ctz64(t);
    		cout << dec << bpos << ", ";
    		clrlsb64(t);
    	}
    	cout << "popcnt = " << popcnt << endl << endl;
    }
	return 0;
}
