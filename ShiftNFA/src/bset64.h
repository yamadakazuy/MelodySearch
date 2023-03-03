/*
 * bset64.h
 *
 *  Created on: 2023/02/24
 *      Author: sin
 */

#ifndef BSET64_H_
#define BSET64_H_


#include <iostream>
#include <string>
#include <cinttypes>


using std::cout;
using std::endl;
using std::string;


struct bset64 {
	uint64_t bits;

public:
	static constexpr unsigned int BIT_LENGTH = 64;

public:
	bset64(void) : bits(0) {}
	bset64(const uint64_t & intval) : bits(intval) {}

	inline bset64 & operator=(const uint64_t & intval) {
		bits = intval;
		return *this;
	}

	inline bset64 & operator=(const bset64 & bset) {
		bits = bset.bits;
		return *this;
	}

	inline bset64 & set(unsigned int bpos) {
		bits |= (uint64_t(1)<<bpos);
		return *this;
	}

	inline bset64 & clear(unsigned int bpos) {
		bits &= ~(uint64_t(1)<<bpos);
		return *this;
	}

	inline unsigned int ctz() {
		return __builtin_ctzll(bits);
	}

	inline bset64 & clsb() {
		bits &= bits -1;
		return *this;
	}

	inline constexpr explicit operator uint64_t() const {
		return bits;
	}

	inline bset64 & operator<<=(const unsigned int s) {
		bits <<= s;
		return *this;
	}

	inline bset64 & operator&=(const bset64 & b) {
		bits &= b.bits;
		return *this;
	}

	inline bset64 & operator&=(const uint64_t & b) {
		bits &= b;
		return *this;
	}

	inline bset64 & operator|=(const bset64 & b) {
		bits |= b.bits;
		return *this;
	}

	inline bset64 & operator|=(const uint64_t & b) {
		bits |= b;
		return *this;
	}

	inline friend bset64 operator&(const bset64 & a, const bset64 & b) {
		return a.bits & b.bits;
	}

	inline friend bset64 operator|(const bset64 & a, const bset64 & b) {
		return a.bits | b.bits;
	}

	inline friend bool operator!=(const bset64 & a, const uint64_t & b) {
		return a.bits != b;
	}

	inline friend bool operator==(const bset64 & a, const uint64_t & b) {
		return a.bits == b;
	}

	friend std::ostream & operator<<(std::ostream & out, const bset64 & bset) {
		out << "bset64{";
		if ( bset.bits ) {
			int cnt = 0;
			uint64_t val = bset.bits;
			for(unsigned int i = 0; i < BIT_LENGTH; ++i) {
				if ( val & 1 ) {
					if (cnt)
						out << ", ";
					out << std::dec << i;
					++cnt;
				}
				val >>= 1;
			}
		}
		out << "} ";
		return out;
	}
};


#endif /* BSET64_H_ */
