//============================================================================
// Name        : MyNFA.cpp
// Author      : Ayane
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ↓入力"*++--+-++==---#+++b==+-=b+-+--+-+-+-+b+==-+-==b==" "++--+-++==---#+++b==+-=b+-+--+-"
//最後に[-]を追加

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

#include "bset64.h"

using std::cout;
using std::endl;
using std::string;

struct MyNFA {
private:
	/* 整数のビット長により設定する上限 */
	static constexpr unsigned int STATE_LIMIT = 64;
	static constexpr unsigned int ALPHABET_LIMIT 	= 128;  /* =,+,-,#,b */
	#define bit64(x)  (uint64_t(1)<<(x))
	#define bit64(x)  (uint64_t(1)<<(x))

	/* 定数 */
	static constexpr unsigned int TRANSITION_NOT_DEFINED =	0;
	static constexpr unsigned int STATE_IS_NOT_FINAL = 	0;
	static constexpr unsigned int STATE_IS_FINAL 	= 1;

	static constexpr char alphabet[] = "#+-0=b";
	/* 状態は 数字，英大文字を含む空白 (0x20) から _ (0x5f) までの一文字 */
	/* に対応させる正の整数 {0,...,63} の要素に限定. */
	/* 文字は ASCII 文字, char 型の {0,...,127} の要素に限定. */
	bset64 delta[STATE_LIMIT][ALPHABET_LIMIT];	/* 遷移関数 : Q x Σ -> 2^Q*/
	bset64  initials; 								/* 初期状態 */
	bset64 finals;			 					/* 最終状態 */
	unsigned int size;
	bset64 current;                           /* 現在の状態の集合　*/

public:
	/* パターン文字列から nfa を初期化 */
	MyNFA(string melody) {
		size = 0;
		for(const char & c : melody) {
			if ( c != '*' )
				++size;
		}
		//std::cout << endl << "statesize = " << statesize << endl;

		/* データ構造の初期化 */
		for(unsigned int i = 0; i < STATE_LIMIT; ++i) {
			for(unsigned int a = 0; a < ALPHABET_LIMIT; ++a) { /* = 1 からでもよいが */
				delta[i][a] = 0; 	/* 空集合に初期化 */
			}
		}
		initials.set(0);
		finals.set(size);

		unsigned int pos = 0;
		for(unsigned int i = 0; i < melody.length(); i++){
			char c = melody[i];

			if(c == '+'){
				delta[pos][(int)'+'] |= bit64(pos+1);
			}else if(c == '#'){
				delta[pos][(int)'#'] |= bit64(pos+1);
			}else if(c == '^'){
				delta[pos][(int)'+'] |= bit64(pos+1);
				delta[pos][(int)'#'] |= bit64(pos+1);
			}else if(c == '-'){
				delta[pos][(int)'-'] |= bit64(pos+1);
			}else if(c == 'b'){
				delta[pos][(int)'b'] |= bit64(pos+1);
			}else if(c == '_'){
				delta[pos][(int)'-'] |= bit64(pos+1);
				delta[pos][(int)'b'] |= bit64(pos+1);
			}else if(c == '='){
				delta[pos][(int)'='] |= bit64(pos+1);
			} else if ( c == '*' ) {
				// VLDC '*'
				for(const char * p = alphabet; *p != 0 ; ++p) {
					delta[pos][(int) *p] |= bit64(pos);
				}
			}
			if ( c != '*') {
				// the state transition for '*' is just the self loop,
				// otherwise it advances the state to the next
				++pos;
			}
		}
	}

	friend std::ostream & operator<<(std::ostream & out, const MyNFA & m) {
		bset64 states(0);
		bool alphabet[MyNFA::ALPHABET_LIMIT];

		for(unsigned int a = 0; a < MyNFA::ALPHABET_LIMIT; ++a) {
			alphabet[a] = false;
		}
		for(unsigned int i = 0; i < MyNFA::STATE_LIMIT; ++i) {
			for(unsigned int a = 0; a < MyNFA::ALPHABET_LIMIT; ++a) {
				if ( m.delta[i][a] != 0 ) {
					states |= bit64(i);
					states |= m.delta[i][a];
					alphabet[a] = true;
				}
			}
		}
		out << "nfa(" << endl;
		out << "states = " << states.str() << endl;
		out << "alphabet = {";
		int count = 0;
		for(unsigned int i = 0; i < ALPHABET_LIMIT; ++i) {
			if ( alphabet[i] == true) {
				if (count)
					printf(", ");
				out << char(i);
				++count;
			}
		}
		out << "}," << endl;

		out << "delta = " << endl;
		out << "state symbol| next" << endl;
		out << "------------+------" << endl;
		for(unsigned int i = 0; i < MyNFA::STATE_LIMIT; ++i) {
			for(unsigned int a = 0; a < MyNFA::ALPHABET_LIMIT; ++a) {
				if ( m.delta[i][a] != 0 ) {
					out << " " << std::setw(3) << i << " ,  " << char(a) << "   | " << m.delta[i][a].str() << endl;
				}
			}
		}
		out << "------------+------" << endl;
		out << "initial state = " << m.initials.str() << endl;
		out << "accepting states = " << m.finals.str() << endl;
		out << ")" << endl;
		return out;
	}

	void reset() {
		current = initials;
	}

	bset64 transfer(char a) {
		bset64 next = 0;
//		for(int i = 0; i < STATE_LIMIT; ++i) {
//			if ((current & (1<<i)) != 0) {
//				if (delta[i][(int)a] != 0) /* defined */
//					next |= delta[i][(int)a];
//				//else /* if omitted, go to and self-loop in the ghost state. */
//			}
//		}
		bset64 state = current;

		while(state != 0){
			unsigned int i = state.ctz();
//			count++;

			if (delta[i][(int)a] != 0){
				next |= delta[i][(int)a];
			}
			state.clsb();
		}
		return current = next;
	}

	int accepting() {
		return (finals & current) != 0;
	}

	long int run(const char * inputstr) {
		const char * ptr = inputstr;
		long long pos = 0;

		reset();
		//std::cout << current.str() ;
		for ( ; *ptr; ++ptr) {
			transfer(*ptr);
			//std::cout << "-" << char(*ptr) << "["<< pos << "]" << "-> "<< current.str() ;
			if ( (finals & current) != 0 ) break;
			pos++;
		}
		//std::cout << endl;

		if (accepting()) {
			return pos;
		} else {
			return -1;
		}
	}

};
/*
int main(int argc, char **argv) {

	string path = "", melody = "";

//	count = 0;

	bool verbose_mode = false; // true で印字出力多め
	if (argc >= 3) {
		for (int i = 1; i < argc; ++i) {
			//if (string("-t") == string(argv[i]) ) {
			//	test_mode = true;
			//	i += 1;
			//} else
			if (string("-v") == string(argv[i]) ) {
				verbose_mode = true;
			} else {
				if (path.length() == 0)
					path = argv[i];
				else
					melody = argv[i];
			}
		}
	} else {
		cout << "requires dirpath melody" << endl;
		exit(1);
	}

	cout << "search " << melody << " for .cont in " << path << " by MyNFA2." << endl;

	int initial = 0;
	int final = melody.length();
	nfa p(melody, initial, final);

//	print(&M);
//
//	char* input = &*path.begin();
//	run(&M, input);

	unsigned long hitcounter= 0, filecounter = 0;
	unsigned long search_micros = 0, total_millis = 0;

	auto start_total = std::chrono::system_clock::now(); // 計測開始時刻
	for (const fsys::directory_entry &entry : fsys::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if (entry.path().string().ends_with(".cont")) {
			filecounter += 1;
			if ( verbose_mode )
				cout << filecounter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)),
					std::istreambuf_iterator<char>());

			char* input= &*text.begin();

			auto start_search = std::chrono::system_clock::now();
			if(p.run(input) != -1){
				hitcounter++;
				if ( verbose_mode )
					cout << "match , " << p.run(input) << endl;
			} else {
				if ( verbose_mode )
					cout << "no match" << endl;
			}
			auto stop_search = std::chrono::system_clock::now(); 	// 計測終了時刻
			search_micros += std::chrono::duration_cast<std::chrono::microseconds >(stop_search - start_search).count(); // ミリ秒に変換
		}
	}

	auto stop_total = std::chrono::system_clock::now(); 	// 計測終了時刻
	total_millis += std::chrono::duration_cast<std::chrono::milliseconds >(stop_total - start_total).count(); // ミリ秒に変換

	cout << "hits = " << hitcounter << " among " << filecounter << " files." << endl;
	cout << "It took " << search_micros << " micros in search, totaly "<< total_millis << " milli seconds." << endl;

	return 0;
}
*/
