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

	static constexpr char alphabet[] = "#+-=b";
	static bool char_match(const char a, const char b) {
		if ( a == b )
			return true;
		if ( a == '^' ) {
			if ( b == '+' or b == '#')
				return true;
		} else if ( a == '_' ) {
			if ( b == '-' or b == 'b')
				return true;
		}
		if ( b == '^' ) {
			if ( a == '+' or a == '#')
				return true;
		} else if ( b == '_' ) {
			if ( a == '-' or a == 'b')
				return true;
		}
		return false;
	}

public:
	NaiveSearcher(const string & melody) {
		size = 0;
		if ( melody.length() == 0 ) {
			pattern.clear();
			return;
		}
		stringstream ss(melody);
		string item;
		while ( std::getline(ss,item,'*') ) {
			//cout << item << ", ";
			if ( item.length() == 0 ) {
				if ( pattern.empty() ) {
					// empty prefix
					pattern.push_back(item);
				}
			} else
				pattern.push_back(item);
		}
		if ( melody.back() == '*' and ! pattern.back().empty())
			pattern.push_back("");
		//cout << endl;
		/*
		for(const auto & s : pattern) {
			cout << s << ", ";
		}
		cout << endl;
		*/
	}

	friend ostream & operator<<(ostream & out, const NaiveSearcher & m) {
		out << "[";
		if (m.pattern.front().size() == 0 ) {
			out << "*";
		} else {
			out << m.pattern.front();
		}
		for(unsigned int i = 1; i < m.pattern.size() - 1; ++i) {
			out << m.pattern[i] << "*";
		}
		if ( m.pattern.back().size() != 0 ) {
			out << m.pattern.back();
		}
		out << "] ";
		return out;
	}

	long run(const char * text) {
		unsigned long pos = 0;
		unsigned int subid;
		//long match_starts = -1;
		if ( !pattern.front().empty() ) {
			if ( std::basic_string_view(text).starts_with(pattern.front()) ) {
				pos += pattern.front().length();
				match_starts = 0;
			} else {
				//cout << "failed at the non-empty prefix" << endl;
				//cout << pos << " " << pattern.front() << endl;
				return -1;
			}
		}
		for( subid = 1; subid < pattern.size() - 1 ; ++subid ) {
			const string & substr = pattern[subid];
			for( ; pos < strlen(text); ++pos) {
				unsigned long shift;
				bool failed = false;
				for( shift = 0; shift < substr.length() ; ++shift) {
					if ( ! char_match(text[pos+shift], substr[shift]) ) {
						failed = true;
						break;
					}
				}
				if ( !failed ) {
					// substr match found.
					//cout << "(" << pos << ", " << pos+shift << ") ";
					pos += shift;
					break;
				}
			}
			if ( ! (pos < strlen(text)) )
				return -1;
		}
		if ( !pattern.back().empty() ) {
			if ( std::basic_string_view(text + pos).ends_with(pattern.back()) ) {
				pos += pattern.back().length();
			}
			//cout << "failed at the non-empty suffix" << endl;
			return -1;
		}
		return pos-1;
	}
};

#endif /* SRC_MYNAIVE_H_ */
