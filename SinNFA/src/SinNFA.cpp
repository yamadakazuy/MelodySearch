//============================================================================
// Name        : SinNFA.cpp
// Author      : Sin Shimozono
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

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
#include <cinttypes>
// requires C++20
#include <filesystem>

//C++11/C++20
#include <chrono>

using std::cout;
using std::endl;
using std::string;

struct bset64 {
	uint64_t bits;

	static constexpr unsigned int UPPER_LIMIT = 64;

public:
	bset64(void) : bits(0) {}
	bset64(const uint64_t & intval) : bits(intval) {}

	bset64 & set(unsigned int bpos) {
		bits |= (uint64_t(1)<<bpos);
		return *this;
	}

	bset64 & clear(unsigned int bpos) {
		bits &= ~(uint64_t(1)<<bpos);
		return *this;
	}

	/*
	bset64 & operator=(uint64_t val) {
		bits = val;
		return *this;
	}
	 */

	constexpr explicit operator uint64_t() const {
		return bits;
	}

	friend std::ostream & operator<<(std::ostream & out, const bset64 & bset) {
		out << "bset64{";
		if ( bset.bits ) {
			int cnt = 0;
			uint64_t val = bset.bits;
			for(int i = 0; i < UPPER_LIMIT; ++i) {
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

struct nfa {
private:
	/* 整数のビット長により設定する上限 */
	static constexpr unsigned int STATE_LIMIT = 64;
	static constexpr unsigned int ALPHABET_LIMIT = 128;  /* =,+,-,#,b のアスキーコードを含む範囲 */

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
	bset64 final_states;			 					/* 最終状態 */
	int size;
	bset64 current;                           /* 現在の状態の集合　*/

	nfa(string melody) : final_states(0), current(1) {
		define(melody);
	}


	/* 文字列から nfa を初期化 */
	void define(const string & melody) {
		size = melody.length() + 1;

		/* データ構造の初期化 */
		staybits = 0;
		for(int asc = 0; asc < ALPHABET_LIMIT; ++asc) {
			advancebits[asc] = 0;
		}
		final_states = 0;

		for(int i = 0; i <= size; i++){
			char c = melody[i];

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
//			count++;

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
			cout << "match , " << find << endl;
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
//	cout << "search " << melody << " for .cont in " << path << endl;

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
//	cout << "counter = " << count << endl;

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
