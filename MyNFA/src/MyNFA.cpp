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

typedef uint64_t bset64; 	/* 符号なし64bit整数型をビット表現で集合として使用する */
typedef struct {
	/* 状態は 数字，英大文字を含む空白 (0x20) から _ (0x5f) までの一文字 */
	/* に対応させる正の整数 {0,...,63} の要素に限定. */
	/* 文字は ASCII 文字, char 型の {0,...,127} の要素に限定. */
	bset64 delta[STATE_LIMIT][ALPHABET_LIMIT];	/* 遷移関数 : Q x Σ -> 2^Q*/
	int  initial; 								/* 初期状態 */
	int finals;			 					/* 最終状態 */
	bset64 current;                           /* 現在の状態の集合　*/
} nfa;

#define bit64(x)  (uint64_t(1)<<(x))

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

/* 定数 */
#define TRANSITION_NOT_DEFINED 	0
#define STATE_IS_NOT_FINAL 		0
#define STATE_IS_FINAL 			1

/* 文字列から nfa を初期化 */
void nfa_define(nfa * mp, string melody, int initial, int finals) {
	int statesize = melody.length();
	//std::cout << endl << "statesize = " << statesize << endl;

	/* データ構造の初期化 */
	for(int i = 0; i < STATE_LIMIT; ++i) {
		for(int a = 0; a < ALPHABET_LIMIT; ++a) { /* = 1 からでもよいが */
			mp->delta[i][a] = 0; 	/* 空集合に初期化 */
		}
	}
	mp->finals = 0;

	for(int i = 0; i <= statesize; i++){
		char c = melody[i];

		if(i == 0){
			mp->delta[i][(int)'+'] |= bit64(i);
			mp->delta[i][(int)'#'] |= bit64(i);
			mp->delta[i][(int)'-'] |= bit64(i);
			mp->delta[i][(int)'b'] |= bit64(i);
			mp->delta[i][(int)'='] |= bit64(i);
			mp->delta[i][(int)'*'] |= bit64(i);
		}

		if(i == statesize){
			mp->delta[i][(int)'+'] |= bit64(i);
			mp->delta[i][(int)'#'] |= bit64(i);
			mp->delta[i][(int)'-'] |= bit64(i);
			mp->delta[i][(int)'b'] |= bit64(i);
			mp->delta[i][(int)'='] |= bit64(i);
		}

		if(c == '+'){
			mp->delta[i][(int)'+'] |= bit64(i+1);
		}else if(c == '#'){
			mp->delta[i][(int)'#'] |= bit64(i+1);
		}else if(c == '^'){
			mp->delta[i][(int)'+'] |= bit64(i+1);
			mp->delta[i][(int)'#'] |= bit64(i+1);
		}else if(c == '-'){
			mp->delta[i][(int)'-'] |= bit64(i+1);
		}else if(c == 'b'){
			mp->delta[i][(int)'b'] |= bit64(i+1);
		}else if(c == '_'){
			mp->delta[i][(int)'-'] |= bit64(i+1);
			mp->delta[i][(int)'b'] |= bit64(i+1);
		}else if(c == '='){
			mp->delta[i][(int)'='] |= bit64(i+1);
		}
//		for(int a = 0; a < ALPHABET_LIMIT; ++a) {
//			if ( mp->delta[i][(int)a] )
//				std::cout << "(" << i << ", " << (char) a << ", " << mp->delta[i][(int)a] << ")" << endl;
//		}
	}
	mp->initial = initial;
	mp->finals |= 1<<finals;
}

void nfa_print(nfa * mp) {
	bset64 states;
	bool alphabet[ALPHABET_LIMIT];
	char buf[160];

	states = 0;
	for(int a = 0; a < ALPHABET_LIMIT; ++a) {
		alphabet[a] = false;
	}
	for(int i = 0; i < STATE_LIMIT; ++i) {
		for(int a = 0; a < ALPHABET_LIMIT; ++a) {
			if ( mp->delta[i][a] ) {
				//std::cout << "(" << i << ", " << (char) a << ", " << mp->delta[i][a] << ")" << endl;
				states |= bit64(i);
				states |= mp->delta[i][a];
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
			if ( mp->delta[i][a] ) {
				printf("  %d  ,  %c   | %s \n", i, a, bset64_str(mp->delta[i][a], buf));
			}
		}
	}
	printf("------------+------\n");
	printf("initial state = %x\n", mp->initial);
	printf("accepting states = %s\n", bset64_str(mp->finals, buf));
	printf(")\n");
	fflush(stdout);
}

void nfa_reset(nfa * mp) {
	mp->current = 1<<mp->initial;
}

bset64 nfa_transfer(nfa * mp, char a) {
	bset64 next = 0;
	for(int i = 0; i < STATE_LIMIT; ++i) {
		if ((mp->current & (1<<i)) != 0) {
			if (mp->delta[i][(int)a] != 0) /* defined */
				next |= mp->delta[i][(int)a];
			//else /* if omitted, go to and self-loop in the ghost state. */
		}
	}
	return mp->current = next;
}

int nfa_accepting(nfa * mp) {
	return (mp->finals & mp->current) != 0;
}

int nfa_run(nfa * mp, char * inputstr) {
	char * ptr = inputstr;
//	char buf[128];
	int find = 0;

//	printf("run on '%s' :\n", ptr);
	nfa_reset(mp);
//	printf("     -> %s", bset64_str(mp->current, buf));

	for ( ; *ptr; ++ptr) {
		nfa_transfer(mp, *ptr);
//		printf(", -%c-> %s", *ptr, bset64_str(mp->current, buf));
		if((mp->finals & mp->current) != 0) break;
		find++;
	}

	if (nfa_accepting(mp)) {
		cout << "match , " << find << endl;
		fflush(stdout);
		return STATE_IS_FINAL;
	} else {
		cout << "no match" << endl;
		fflush(stdout);
		return STATE_IS_NOT_FINAL;
	}
}

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
	int finals = melody.length();
	nfa M;
	nfa_define(&M, melody, initial, finals);
//	nfa_print(&M);
//
//	char* input = &*path.begin();
//	nfa_run(&M, input);

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

			nfa_run(&M, input);
		}
	}

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
