//============================================================================
// Name        : NewNFA.cpp
// Author      : Ayane, Sin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
//#include <cstdio>
#include <cinttypes>

// requires C++20
#include <filesystem>

//C++11/C++20
#include <chrono>

using std::cout;
using std::endl;
using std::string;
namespace fsys = std::filesystem;

#define bit64(x)  (uint64_t(1)<<(x))

typedef uint64_t bset64; 	/* 符号なし64bit整数型をビット表現で集合として使用する */

struct LineNFA {
private:
	/* 状態は 1LL<<0 から 1LL<<63 の63 種類*/
	/* 遷移を定義できる文字は ASCII char { =,+,-,#,b　} に限定. */
	static constexpr unsigned int STATE_LIMIT = 64;
	static constexpr unsigned int ALPHABET_LIMIT = 8;  /* = -> 1, # -> 2, + -> 3, ^ -> 4, b -> 7, - -> 6, _ -> 5 */
	static constexpr unsigned char indexalpha[] = "\0=#+^_-b";
	static constexpr unsigned int  alphaindex[] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

			0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 3, 0, 6, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,

			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5,

			0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};
	/* 定数 */
	static constexpr unsigned int TRANSITION_NOT_DEFINED = 0;
	static constexpr unsigned int STATE_IS_NOT_FINAL = 0;
	static constexpr unsigned int STATE_IS_FINAL = 1;

	//uint8_t alphaindex[256];
	//char indexalpha[ALPHABET_LIMIT];
	//bset64 delta[STATE_LIMIT][ALPHABET_LIMIT];	/* 遷移関数 : Q x Σ -> 2^Q*/
	bset64 stayers[ALPHABET_LIMIT];
	bset64 ascenders[ALPHABET_LIMIT];
	bset64 initial; 								/* 初期状態 */
	bset64 final_states;				 					/* 最終状態 */
	int size;
	bset64 current;                           /* 現在の状態の集合　*/

	static std::ostream & bset64_out(std::ostream & out, bset64 bits) {
		out << "{";
		int cnt = 0;
		for(unsigned int i = 0; i < STATE_LIMIT; ++i) {
			if (bits>>i & 1) {
				if (cnt)
					out << ", ";
				out << std::dec << i;
				++cnt;
			}
		}
		out << "}";
		return out;
	}

public:
	LineNFA(string melody, uint64_t initial, bset64 finals) {
		define(melody, initial, finals);
	}


	/* 文字列から nfa を初期化 */
	void define(string melody, uint64_t init, bset64 fin) {
		size = melody.length();
		//std::cout << endl << "statesize = " << statesize << endl;

		/* データ構造の初期化 */
		//for(unsigned int i = 0; i < STATE_LIMIT; ++i) {
			for(unsigned int a = 0; a < ALPHABET_LIMIT; ++a) { /* = 1 からでもよいが */
				stayers[a] = 0LL;
				ascenders[a] = 0LL;
				//delta[i][a] = 0; 	/* 空集合に初期化 */
			}
		//}

		for(int i = 0; i < size; i++){
			if(i == 0){
				for(unsigned int a = 0; a < ALPHABET_LIMIT; ++a) {
					stayers[a] = bit64(i);
				}
			}
			if(i == size){
				for(unsigned int a = 0; a < ALPHABET_LIMIT; ++a) {
					stayers[a] = bit64(i+1);
				}
			}
			char c = melody[i];
			switch( int(c) ) {
			case '^':
				ascenders[alphaindex[int('+')]] |= bit64(i);
				ascenders[alphaindex[int('#')]] |= bit64(i);
				break;
			case '_':
				ascenders[alphaindex[int('-')]] |= bit64(i);
				ascenders[alphaindex[int('b')]] |= bit64(i);
				break;
			case '=':
			case '#':
			case '+':
			case 'b':
			case '-':
				ascenders[alphaindex[int(c)]] |= bit64(i);
				break;
			}

		}
		initial = 1LL<<initial;
		final_states |= fin;
	}

	friend std::ostream & operator<<(std::ostream & out, const LineNFA & nfa) {
		bset64 states = 0;

		out << "nfa(\n";
		out << "states = ";
		for(int i = 0; i < nfa.size; ++i) {
			states |= bit64(i);
		}
		bset64_out(out, states);
		out << "alphabet = {";
		out << (nfa.indexalpha+1);
		out << "}, ";

		return out;
	}

	void reset() {
		current = 1<<initial;
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
			int i = __builtin_ctz(state);
			if (delta[i][(int)a] != 0){
				next |= delta[i][(int)a];
			}
			state &= state - 1;
		}
		return current = next;
	}

	int accepting() {
		return (final & current) != 0;
	}

	int run(char * inputstr) {
		char * ptr = inputstr;
	//	char buf[128];
		int find = 0;

	//	printf("run on '%s' :\n", ptr);
		reset();
	//	printf("     -> %s", bset64_str(mp->current, buf));

		for ( ; *ptr; ++ptr) {
			transfer(*ptr);
	//		printf(", -%c-> %s", *ptr, bset64_str(mp->current, buf));
			if((final & current) != 0) break;
			find++;
		}

		if (accepting()) {
			cout << "match , " << find - size + 1 << endl;
			fflush(stdout);
			return STATE_IS_FINAL;
		} else {
			cout << "no match" << endl;
			fflush(stdout);
			return STATE_IS_NOT_FINAL;
		}
	}

};

int main(int argc, char **argv) {

	string path, melody;

	if (argc >= 3) {
		path = argv[1];
		melody = argv[2];
	} else {
		cout << "requires path searchfile " << endl;
		exit(1);
	}
	cout << "search " << melody << " for .cont in " << path << endl;

	int initial = 0;
	int final = melody.length();
	nfa p(melody, initial, final);

//	print(&M);
//
//	char* input = &*path.begin();
//	run(&M, input);

	unsigned int counter = 0;
	auto start = std::chrono::system_clock::now(); // 計測開始時刻

	for (const fsys::directory_entry &entry : fsys::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if (entry.path().string().ends_with(".cont")) {
			counter += 1;
			cout << counter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)),
					std::istreambuf_iterator<char>());

			char* input= &*text.begin();

			p.run(input);
		}
	}

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
