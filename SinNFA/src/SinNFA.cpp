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

namespace fsys = std::filesystem;


struct bset64 {
	uint64_t bits;

	static constexpr unsigned int UPPER_LIMIT = 64;

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

	bset64 & set(unsigned int bpos) {
		bits |= (uint64_t(1)<<bpos);
		return *this;
	}

	inline bset64 & clear(unsigned int bpos) {
		bits &= ~(uint64_t(1)<<bpos);
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

	inline friend bset64 operator&(const bset64 & a, const bset64 b) {
		return a.bits & b.bits;
	}

	inline friend bset64 operator|(const bset64 & a, const bset64 b) {
		return a.bits | b.bits;
	}

	inline friend bool operator!=(const bset64 & a, const uint64_t b) {
		return a.bits != b;
	}

	friend std::ostream & operator<<(std::ostream & out, const bset64 & bset) {
		out << "bset64{";
		if ( bset.bits ) {
			int cnt = 0;
			uint64_t val = bset.bits;
			for(unsigned int i = 0; i < UPPER_LIMIT; ++i) {
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
	nfa(const string & melody) : initial_state(0), final_states(0), current(0) {
		initial_state.set(0);
		string patt;
		for(unsigned int i = 0; i < melody.size(); ++i) {
			if ( i == 0 and melody[i] == '*' )
				continue;
			if ( i == melody.size() - 1 and melody[i] == '*' )
				continue;
			patt.push_back(melody[i]);
		}
		size = patt.size();
		final_states.set(size);
		define(patt);
	}


	/* 文字列から nfa を初期化 */
	void define(const string & melody) {
		/* データ構造の初期化 */
		staybits.set(0);
		staybits.set(size);

		for(unsigned int ascii = 0; ascii < ALPHABET_LIMIT; ++ascii) {
			advancebits[ascii] = 0;
		}
		for(int i = 0; i <= size; i++){
			char c = melody[i];
			switch ( c ) {
			case '*':
				staybits.set(i);
				break;
			case '=':
			case '+':
			case '-':
			case '#':
			case 'b':
				advancebits[int(c)].set(i);
				break;
			case '^':
				advancebits[int('+')].set(i);
				advancebits[int('#')].set(i);
				break;
			case '_':
				advancebits[int('-')].set(i);
				advancebits[int('b')].set(i);
				break;
			}
		}
	}

	friend std::ostream & operator<<(std::ostream & out, const nfa & m) {
		out << "NFA(states = " << m.current;
		out << ", staybits = " << m.staybits << ", " << endl;
		for(unsigned int ascii = 0; ascii < ALPHABET_LIMIT; ++ascii) {
			if ( uint64_t(m.advancebits[ascii]) != 0 ) {
				out << char(ascii) << " : " << m.advancebits[ascii] << endl;
			}
		}
		out << "final states = " << m.final_states << ") " << endl;
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

	long long run(const char * inputstr) {
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

int main(int argc, char **argv) {

	string path, melody;

	if (argc >= 3) {
		path = argv[1];
		melody = argv[2];
	} else {
		cout << "requires path searchfile " << endl;
		exit(1);
	}

	nfa m(melody);

	cout << "search " << melody << " for .cont in " << path << endl;
	//cout << "by NFA " << m << endl;

	long filecounter = 0;
	long hitcounter = 0;
	long bytecounter = 0;
	long long search_micros = 0, total_millis = 0;

	auto start_total = std::chrono::system_clock::now(); // 計測開始時刻
	for (const fsys::directory_entry &entry :
			fsys::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if (entry.path().string().ends_with(".cont")) {
			filecounter ++;
			//cout << filecounter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)),
					std::istreambuf_iterator<char>());

			//char* input= &*text.begin();
			bytecounter += text.length();
			auto start_search = std::chrono::system_clock::now(); // 計測開始時刻
			long long pos = m.run(text.c_str());
			if ( pos >= 0 ){
				hitcounter ++;
				cout << filecounter << " @ " << pos << " in " << entry.path().string() << endl;
			}
			auto stop_search = std::chrono::system_clock::now(); 	// 計測終了時刻
			search_micros += std::chrono::duration_cast<std::chrono::microseconds >(stop_search - start_search).count(); // ミリ秒に変換
		}
	}
	auto stop_total = std::chrono::system_clock::now(); 	// 計測終了時刻
	total_millis += std::chrono::duration_cast<std::chrono::milliseconds >(stop_total - start_total).count(); // ミリ秒に変換

	cout << hitcounter << " hits " << filecounter << " files " << bytecounter << " bytes "
			<< search_micros << " micros "<< total_millis << " mills." << endl;

	return 0;
}
