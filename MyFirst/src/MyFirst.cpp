//============================================================================
// Name        : MyFirst.cpp
// Author      : Ayane
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
// requires C++20
#include <filesystem>

using std::cout;
using std::endl;
using std::string;
namespace fsys = std::filesystem;

/*
 * 力まかせ法によりtextからmelodyを見つけ出す
 *
 * text：楽曲内の音程列
 * text_len：音程列の文字数
 * melody：ユーザーが入力したメロディ概形文字列
 * melody_len：melodyの文字数
 * 戻り値：melodyが見つかった場合はその位置、melodyが見つからない場合は-1
 */

int NaiveSearch(string text, int text_len, string melody, int melody_len) {
	int shift; /* melodyをずらす文字数 */
	int melody_pos; /* 比較位置 */

	/* melodyが存在しうるテキストの位置にmelodyをずらすループ */
	for (shift = 0; shift <= text_len - melody_len; shift++) {

		/* melodyの全文字と比較を行うループ */
		for (melody_pos = 0; melody_pos < melody_len; melody_pos++) {

			//^が入力されたとき//
			if (melody[melody_pos] == '^') {
				//cout << text[shift + melody_pos] << endl;
				if (text[shift + melody_pos] == '#'
						|| text[shift + melody_pos] == '+') {
					continue;
				} else {
					break;
				}
			}
			//_が入力されたとき//
			else if (melody[melody_pos] == '_') {
				if (text[shift + melody_pos] == 'b'
						|| text[shift + melody_pos] == '-') {
					continue;
				} else {
					break;
				}
			}

			/* パターンをずらした時に同じ位置になる文字同士を比較 */
			else if (text[shift + melody_pos] == melody[melody_pos]) {

				/* 文字が一致した場合 */
				if (melody_pos == melody_len - 1) {
					/* パターンの最後の文字と一致した場合 */

					/* パターンが見つかったのでパターンが存在する位置を返却 */
					return shift;
				}
			} else {
				/* 文字が不一致した場合 */

				/* パターンは絶対に存在しないので比較中止 */
				break;
			}
		}
	}

	/* パターンが見つからなかった場合は-1を返却 */
	return -1;
}

int main(const int argc, char *argv[]) {
//	reading_file.open(filename, std::ios::in)
//
//	// １文字ずつ最後まで読み込み
//	while(reading_file.get(text)) std::cout << text;
//
//	find = NaiveSearch(text, strlen(text), argv, strlen(argv));

	string path, melody;
	if (argc >= 3) {
		path = argv[1];
		melody = argv[2];
	} else {
		cout << "requires path searchfile " << endl;
		exit(1);
	}
	cout << "search " << melody << " for .contour in " << path << endl;

	unsigned int counter = 0;
	for (const fsys::directory_entry &entry : fsys::recursive_directory_iterator(
			path)) {
		if (entry.is_directory())
			continue;
		if (entry.path().string().ends_with(".contour")) {
			counter += 1;
			cout << counter << " " << entry.path().string() << endl;
			std::ifstream ifs(entry.path().string());
			string text((std::istreambuf_iterator<char>(ifs)),
					std::istreambuf_iterator<char>());

			int find = NaiveSearch(text, text.length(), melody,
					melody.length());

			if (find != -1) {
				cout << melody << ", " << find << endl;
			} else {
				cout << "no match" << endl;
			}
		}
	}

//	int buf_size = 81;
//
//	char str[buf_size];
//	    if (searchfile.fail()) {
//	        std::cerr << "Failed to open file." << std::endl;
//	        return -1;
//	    }
//	    while (searchfile.getline(str, buf_size)) {
//	    	find = NaiveSearch(str, strlen(str), argv, strlen(argv));
//	    }
//	    return 0;
//	}
//
//	if (find != -1) {
//		cout << text << ", " << find << endl;
//	} else {
//		cout << "no match" << endl;
//	}

	return 0;
}
