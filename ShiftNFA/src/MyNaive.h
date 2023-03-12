//============================================================================
// Name        : MyFirst2.cpp
// Author      : Ayane
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#ifndef SRC_MYNAIVE_H_
#define SRC_MYNAIVE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>

using namespace std;
using std::cout;
using std::endl;
using std::string;

/*
 * 力まかせ法によりtextからmelodyを見つけ出す
 *
 * text：楽曲内の音程列
 * text_len：音程列の文字数
 * melody：ユーザーが入力したメロディ概形文字列
 * melody_len：melodyの文字数
 * 戻り値：melodyが見つかった場合はその位置、melodyが見つからない場合は-1
 */

struct NaiveSearcher {
private:
	vector<string> pattern; // the first and last strings are prefix and suffix.
	unsigned int size;

public:
	NaiveSearcher(const string & melody) {
		size = 0;
		if ( melody.length() == 0 ) {
			pattern.clear();
			return;
		}
		unsigned int i = 0;
		while (melody[i] != 0) {
			const char c = melody[i];
			if ( c == '*' ) {
				if ( pattern.empty() or ! pattern.back().empty() ) {
					pattern.push_back("");
					++size;
				}
			} else {
				if ( pattern.empty() ) {
					pattern.push_back("");
				}
				pattern.back() += c;
				++size;
			}
			++i;
		}
	}

	friend ostream & operator<<(ostream & out, const NaiveSearcher & m) {
		out << "[";
		for(unsigned int i = 0; i < m.pattern.size(); ++i) {
			if ( i != 0 )
				out << ", ";
			out << m.pattern[i];
		}
		out << "] ";
		return out;
	}


	long search(const char * text) {
		return -1;
	}
};

#endif /* SRC_MYNAIVE_H_ */
