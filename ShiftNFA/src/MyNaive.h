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
#include <sstream>
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
	vector<string> pattern; // store the pattern in normalized form. the first one is prefix, the last one is suffix.
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

	// return the start position of suff in txt
	// returns -1 if otherwise
	static long suffix_match(const string & txt, const string & suff) {
		//cout << txt << " == " << suff << " ?" << endl;
		unsigned long len, ia = txt.length(), ib = suff.length();
		if ( ia < ib )
			return -1;
		for(len = 0; len < suff.length(); ++len) {
			--ia, --ib;
			if ( ! char_match(txt[ia], suff[ib]) ) {
				//cout << " failed " << endl;
				return -1;
			}
		}
		//cout <<  len <<endl;
		if ( len == suff.length() )
			return (long) ia;
		return -1;
	}

	// find the start position of sub in txt from start to end - 1
	// returns -1 if there is no such poisition
	static long substr_match(const string & txt, const unsigned long & start, const unsigned long & end, const string & sub) {
		unsigned long pos, len;
		for(pos = start; pos < end; ++pos) {
			for(len = 0; len < sub.length(); ++len) {
				// test sub[0, len+1)
				if ( !(pos + len < end) ) {
					// abandon if txt[pos,end) is already shorter than sub.length()
					return -1;
				}
				if ( ! char_match(txt[pos+len], sub[len]) )
					break;
			}
			if ( len == sub.length() )
				return pos;
		}
		return -1;
	}

public:
	NaiveSearcher(const string & melody) {
		size = 0;

		if ( melody.length() == 0 )
			return;

		if ( melody.front() == '*' )
			pattern.push_back("");
		ostringstream ss;
		for(const auto & ch : melody) {
			if ( ch == '*' and ss.str().length() > 0 ) {
				pattern.push_back(ss.str()); // new substring segment
				ss.str("");
				ss.clear();
			} else if ( ch != '*' ) {
				ss <<  ch;
			}
		}
		pattern.push_back(ss.str());
	}

	friend ostream & operator<<(ostream & out, const NaiveSearcher & m) {
		out << "[";
		if ( m.pattern.front().length() > 0 )
			out <<  m.pattern.front();
		out << "*";
		for (unsigned int i = 1; i < m.pattern.size() - 1 ; ++i) {
			out << m.pattern[i] << "*";
		}
		if ( m.pattern.back().length() > 0 )
			out <<  m.pattern.back();
		out << "] ";
		return out;
	}


	long run(const string & text) const { return search(text); }

	long search(const string & text) const {
		long textpos = 0;
		long textend = text.length();

		if ( text.empty() )
			return -1;
		if ( pattern.empty() )
			return textpos;
		//cout << "non empty" << endl;

		// process prefix
		if ( pattern.front().length() > 0 ) {
			long pos = substr_match(text, 0, pattern.front().length(), pattern.front());
			if ( pos != 0 )
				return -1;
			textpos = pattern.front().length();
		}

		// process suffix
		if ( pattern.back().length() > 0 ) {
			textend = suffix_match(text, pattern.back());
			if ( textend == -1 )
				return -1;
		}

		//cout << "text end " << text_end << " patt_end " << patt_end << endl;
		unsigned int segid;
		for(segid = 1; segid < pattern.size() - 1; ++segid) {
			//cout << segid << " " << pattern[segid] << " text = " << text << endl;
			long pos = substr_match(text, textpos, textend, pattern[segid]);
			if ( pos != -1 ) {
				textpos = pos + pattern[segid].length();
				//cout << " pos = " << pos << endl;
				continue;
			} else {
				//cout << " npos = " << pos << endl;
				break;
			}

		}
		//cout << " segid " << segid << endl;

		if ( segid != pattern.size() - 1 )
			return -1;
		if (pattern.back().length() > 0 )
			return text.length();
		return textpos - 1;
	}
};

#endif /* SRC_MYNAIVE_H_ */
