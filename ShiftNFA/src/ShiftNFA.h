/*
 * ShiftNFA.h
 *
 *  Created on: 2023/03/02
 *      Author: Sin Shimozono
 */

#ifndef SRC_SHIFTNFA_H_
#define SRC_SHIFTNFA_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cinttypes>

using std::cout;
using std::endl;
using std::string;

#include "bset64.h"

struct ShiftNFA {
private:
	/* 整数のビット長により設定する上限 */
	static constexpr unsigned int STATE_LIMIT = 64;
	/* =,+,-,#,b のアスキーコードを含む範囲 */
	static constexpr unsigned int ALPHABET_LIMIT = 128;

	/* 定数 */
	/* 状態は 数字，英大文字を含む空白 (0x20) から _ (0x5f) までの一文字 */
	/* に対応させる正の整数 {0,...,63} の要素に限定. */
	/* 文字は ASCII 文字, char 型の {0,...,127} の要素に限定. */
	//bset64 delta[STATE_LIMIT][ALPHABET_LIMIT];	/* 遷移関数 : Q x Σ -> 2^Q*/
	enum SPECIAL_STATE {
		INITIAL_STATE = 0,
		TRANSITION_NOT_DEFINED = 0,
		STATE_IS_NOT_FINAL = 0,
		STATE_IS_FINAL = 1,
	};

	bset64 staybits;
	bset64 advancebits[ALPHABET_LIMIT];
	bset64 initial_state;
	bset64 final_states;			 					/* 最終状態 */
	int size;
	bset64 current;                           /* 現在の状態の集合　*/

public:
	ShiftNFA(const string & melody) : initial_state(0), final_states(0), current(0) {
		initial_state.set(0);
		size = 0;
		for(const char & c : melody) {
			if ( c != '*' )
				++size;
		}
		final_states.set(size);
		define(melody);
	}


	/* 文字列から nfa を初期化 */
	void define(const string & melody) {
		/* データ構造の初期化 */
		staybits.set(0);
		staybits.set(size);

		for(unsigned int ascii = 0; ascii < ALPHABET_LIMIT; ++ascii) {
			advancebits[ascii] = 0;
		}
		unsigned int pos = 0; // position in the pattern whose '*' replaced by zero-length separator
		for(unsigned int i = 0; i < melody.length(); ++i){
			char c = melody[i];
			switch ( c ) {
			case '*':
				staybits.set(pos);
				break;
			case '=':
			case '+':
			case '-':
			case '#':
			case 'b':
				advancebits[int(c)].set(pos);
				break;
			case '^':
				advancebits[int('+')].set(pos);
				advancebits[int('#')].set(pos);
				break;
			case '_':
				advancebits[int('-')].set(pos);
				advancebits[int('b')].set(pos);
				break;
			}
			if ( c != '*' )
				++pos;
		}
	}

	friend std::ostream & operator<<(std::ostream & out, const ShiftNFA & m) {
		bset64 allstates = m.staybits;
		for(unsigned int ascii = 0; ascii < ALPHABET_LIMIT; ++ascii) {
			allstates |= m.advancebits[ascii];
		}
		out << "ShiftNFA(states = " << allstates.str();
		out << ", staybits = " << m.staybits.str() << ", " << endl << "advance bits = " << endl;
		for(unsigned int ascii = 0; ascii < ALPHABET_LIMIT; ++ascii) {
			if ( uint64_t(m.advancebits[ascii]) != 0 ) {
				out << char(ascii) << " : " << m.advancebits[ascii].str() << endl;
			}
		}
		out << "final states = " << m.final_states.str() << ") " << endl;
		return out;
	}

	void reset() {
		current = initial_state;
	}


	bset64 transfer(const char a) {
		bset64 next = current & advancebits[int(a)];
		next <<= 1;
		return current = next | (current & staybits);
	}

	bool accepting() {
		return (final_states & current) != 0;
	}

	long run(const char * inputstr) {
		const char * ptr = inputstr;
		long long pos = 0;
		reset();
		for ( ; *ptr; ++ptr) {
			transfer(*ptr);
			if ( accepting() )
				break;
			++pos;
		}

		if (accepting()) {
			//cout << "match , " << pos << endl;
			return pos;
		} else {
			//cout << "no match" << endl;
			return -1;
		}
	}

};

#endif /* SRC_SHIFTNFA_H_ */
