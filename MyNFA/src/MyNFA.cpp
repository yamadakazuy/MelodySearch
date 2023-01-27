//============================================================================
// Name        : MyNFA.cpp
// Author      : Ayane
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

// ↓入力"*++--+-++==---#+++b==+-=b+-+--+-+-+-+b+==-+-==b==" "++--+-++==---#+++b==+-=b+-+--+-"
//最後に[-]を追加
// version 22/12/06 17:05

#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
// requires C++20
#include <filesystem>

//C++11/C++20
#include <chrono>

using std::cout;
using std::endl;
using std::string;
namespace fsys = std::filesystem;


/* 整数のビット長により設定する上限 */
#define STATE_LIMIT	 			64
#define ALPHABET_LIMIT 			128  /* =,+,-,#,b */
#define bit64(x)  (uint64_t(1)<<(x))
#define bit64(x)  (uint64_t(1)<<(x))

/* 定数 */
#define TRANSITION_NOT_DEFINED 	0
#define STATE_IS_NOT_FINAL 		0
#define STATE_IS_FINAL 			1

long count;

typedef uint64_t bset64; 	/* 符号なし64bit整数型をビット表現で集合として使用する */

struct nfa {
	/* 状態は 数字，英大文字を含む空白 (0x20) から _ (0x5f) までの一文字 */
	/* に対応させる正の整数 {0,...,63} の要素に限定. */
	/* 文字は ASCII 文字, char 型の {0,...,127} の要素に限定. */
	bset64 delta[STATE_LIMIT][ALPHABET_LIMIT];	/* 遷移関数 : Q x Σ -> 2^Q*/
	int  initial; 								/* 初期状態 */
	int final;			 					/* 最終状態 */
	int size;
	bset64 current;                           /* 現在の状態の集合　*/


	char * bset64_str(bset64 bits, char * buf) {
		char * ptr = buf;
		ptr += sprintf(ptr, "{");
		int cnt = 0;
		for(int i = 0; i < STATE_LIMIT; ++i) {
			if (bits>>i & 1) {
				if (cnt) ptr += sprintf(ptr, ", ");
				ptr += sprintf(ptr, "%d", i);
				++cnt;
			}
		}
		sprintf(ptr, "}");
		return buf;
	}

	nfa(string melody, int initial, int final){
		define(melody, initial, final);
	}


	/* 文字列から nfa を初期化 */
	void define(string melody, int init, int fin) {
		size = melody.length();
		//std::cout << endl << "statesize = " << statesize << endl;

		/* データ構造の初期化 */
		for(int i = 0; i < STATE_LIMIT; ++i) {
			for(int a = 0; a < ALPHABET_LIMIT; ++a) { /* = 1 からでもよいが */
				delta[i][a] = 0; 	/* 空集合に初期化 */
			}
		}
		final = 0;

		for(int i = 0; i <= size; i++){
			char c = melody[i];

			if(i == 0){
				delta[i][(int)'+'] |= bit64(i);
				delta[i][(int)'#'] |= bit64(i);
				delta[i][(int)'-'] |= bit64(i);
				delta[i][(int)'b'] |= bit64(i);
				delta[i][(int)'='] |= bit64(i);
				delta[i][(int)'0'] |= bit64(i);
			}

			if(i == size){
				delta[i][(int)'+'] |= bit64(i);
				delta[i][(int)'#'] |= bit64(i);
				delta[i][(int)'-'] |= bit64(i);
				delta[i][(int)'b'] |= bit64(i);
				delta[i][(int)'='] |= bit64(i);
			}

			if(c == '+'){
				delta[i][(int)'+'] |= bit64(i+1);
			}else if(c == '#'){
				delta[i][(int)'#'] |= bit64(i+1);
			}else if(c == '^'){
				delta[i][(int)'+'] |= bit64(i+1);
				delta[i][(int)'#'] |= bit64(i+1);
			}else if(c == '-'){
				delta[i][(int)'-'] |= bit64(i+1);
			}else if(c == 'b'){
				delta[i][(int)'b'] |= bit64(i+1);
			}else if(c == '_'){
				delta[i][(int)'-'] |= bit64(i+1);
				delta[i][(int)'b'] |= bit64(i+1);
			}else if(c == '='){
				delta[i][(int)'='] |= bit64(i+1);
			}
	//		for(int a = 0; a < ALPHABET_LIMIT; ++a) {
	//			if ( mp->delta[i][(int)a] )
	//				std::cout << "(" << i << ", " << (char) a << ", " << mp->delta[i][(int)a] << ")" << endl;
	//		}
		}
		initial = init;
		final |= 1<<fin;
	}

	void print() {
		bset64 states;
		bool alphabet[ALPHABET_LIMIT];
		char buf[160];

		states = 0;
		for(int a = 0; a < ALPHABET_LIMIT; ++a) {
			alphabet[a] = false;
		}
		for(int i = 0; i < STATE_LIMIT; ++i) {
			for(int a = 0; a < ALPHABET_LIMIT; ++a) {
				if ( delta[i][a] ) {
					//std::cout << "(" << i << ", " << (char) a << ", " << mp->delta[i][a] << ")" << endl;
					states |= bit64(i);
					states |= delta[i][a];
					alphabet[a] = true;
					//std::cout << (char) a << " " << (int) alphabet[a] << ", ";
				}
			}
		}
		//std::cout << endl;
		printf("nfa(\n");
		printf("states = %s\n", bset64_str(states, buf));
		printf("alphabet = {");
		int count = 0;
		for(int i = 0; i < ALPHABET_LIMIT; ++i) {
			//std::cout << (int)alphabet[i] << ", ";
			if ( alphabet[i] == true) {
				if (count)
					printf(", ");
				printf("%c", (char) i);
				++count;
			}
		}
		printf("},\n");

		printf("delta = \n");
		printf("state symbol| next\n");
		printf("------------+------\n");
		for(int i = 0; i < STATE_LIMIT; ++i) {
			for(int a = 0; a < ALPHABET_LIMIT; ++a) {
				if ( delta[i][a] ) {
					printf("  %d  ,  %c   | %s \n", i, a, bset64_str(delta[i][a], buf));
				}
			}
		}
		printf("------------+------\n");
		printf("initial state = %x\n", initial);
		printf("accepting states = %s\n", bset64_str(final, buf));
		printf(")\n");
		fflush(stdout);
	}

	void reset() {
		current = 1<<initial;
	}

	bset64 transfer(char a) {
		bset64 next = 0;
		for(int i = 0; i < STATE_LIMIT; ++i) {
			count++;
			if ((current & (1<<i)) != 0) {
				if (delta[i][(int)a] != 0) /* defined */
					next |= delta[i][(int)a];
				//else /* if omitted, go to and self-loop in the ghost state. */
			}
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
	int hit = 0;

	count = 0;

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

			if(p.run(input)== 1){
				hit++;
			}
		}
	}

	cout << "hit = " << hit << endl;
	cout << "counter = " << count << endl;

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
