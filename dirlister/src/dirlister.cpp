//============================================================================
// Name        : dirlister.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

// 個々に std:: を省略できるようにするおまじない（名前空間に取り込む）
using std::cout;
using std::endl;
using std::string;

bool naive_search(const string & text, unsigned long long & pos, const string & patt) {
	unsigned long long i;
	for(i = pos; i < text.length() - patt.length() + 1; ++i) {
		unsigned long long j;
		for(j = 0; j < patt.length() ; ++j) {
			if (text[i + j] != patt[j])
				break;
		}
		if (j == patt.length()) {
			pos = i;
			return true;
		}
	}
	pos = i;
	return false;
}

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	// Windows では \, Linux, MacOS では /
    string path = "..\\smf";  /* \ はエスケープシーケンス開始の意味を持つ文字なので \\ で表す */

    // サンプル：　あるファイルパス（path）に存在するファイルまたはディレクトリパスの名前をすべてプリントする
    cout << "List the names of files in the specified directory path." << endl;
    cout << "path = " << path << endl;
    for (const auto & file : std::filesystem::directory_iterator(path)) {
        cout << file.path() << endl;
    }
    cout << endl << endl;

    // サンプル： あるファイルパス（path）以下のディレクトリにある名前が .txt で終わるファイルすべてについて、
    // その内容（テキスト）を string 型の変数 text にセットし、
    // text に対してパターン検索を行い、ファイル毎に一行で結果を表示する。
    cout << "Show the results for all .txt files under the specified path " << path << endl;
    unsigned long long pos;
    int counter = 0;
    string pattern = "-b#+b#b-=";
	for (const auto & entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.is_directory())
			continue;
		if ( entry.path().string().ends_with(".txt") ) { // ends_with() --- introduced in C++20
			counter += 1;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			// ↑ 一度に文字列型で読み込んでしまう。ファイルサイズがメモリーにのる程度なら問題ない
			pos = 0;
			if ( naive_search(text, pos, pattern) ) {
				cout << counter << " " << entry.path().string() << "\t";
				//cout << text.substr(0, std::min((unsigned int)100, (unsigned int)text.length())) << endl;
				cout << (unsigned long) pos << ", ";
				while ( naive_search(text, pos, pattern) ) {
					cout << (unsigned long) pos << ", ";
					pos += pattern.length();
				}
				cout << endl;
			}
		}
	}

    return EXIT_SUCCESS;
}
