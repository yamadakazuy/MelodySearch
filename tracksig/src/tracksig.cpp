//============================================================================
// Name        : tracksig.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <iomanip>
#include <cinttypes>
using namespace std;

uint32_t get_uint32BE(const char * str) {
		uint32_t res = 0;
		for(uint16_t i = 0; i < 4; ++i) {
			res <<= 8;
			res |= uint8_t(*str);
			++str;
		}
		return res;
}

int main() {
	constexpr char MTrk[] = "MTrk";
	constexpr char MThd[] = "MThd";
	constexpr char XFIH[] = "XFIH";
	constexpr char XFKM[] = "XFKM";

	cout << MTrk << " 0x" << hex << get_uint32BE(MTrk) << endl;
	cout << MThd << " 0x" << hex << get_uint32BE(MThd) << endl;
	cout << XFIH << " 0x" << hex << get_uint32BE(XFIH) << endl;
	cout << XFKM << " 0x" << hex << get_uint32BE(XFKM) << endl;
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
