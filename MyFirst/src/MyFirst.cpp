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

//C++11/C++20
#include <chrono>

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
	int shift; /* melodyをずらす文字数 */ // <-- melody が出現するかどうかテストする位置のこと？
	int melody_pos; /* 比較する位置 */ // melody 中の何文字目を比較するかの位置のこと？

	/* melodyが存在しうるテキストの位置にmelodyをずらすループ */
	for (shift = 0; shift <= text_len - melody_len; shift++) {
		// 照合が melody の最後まで成功した場合、shift は出現位置（先頭）を表す（？）
		/* melodyの全文字と比較を行うループ */
		for (melody_pos = 0; melody_pos < melody_len; melody_pos++) {
			cout << text[shift + melody_pos] << " @ shift = " << shift << ", " << melody[melody_pos] << " @ mel pos = " << melody_pos << endl;
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
				// ここは成功したら continue, 失敗したら break じゃね？
				continue;
			} else {
				break;
			}
			/*
				// 文字が一致した場合
				if (melody_pos == melody_len - 1) {
					// パターンの最後の文字と一致した場合

					// パターンが見つかったのでパターンが存在する位置を返却
					return shift;
				}
			} else {
				// 文字が不一致した場合

				// パターンは絶対に存在しないので比較中止
				break;
			}
			*/
		}
		// 一致が途中で失敗した場合、melody_pos < melody_len で終わる．
		// 途中で失敗しなかった場合、最後に melody_pos++　をした後、melody_pos < melody_len が満たせず終了する．
		// ので
		if ( ! (melody_pos < melody_len) )
			return shift;
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
	bool test_mode = false;
	if (argc >= 3) {
		int i = 1;
		if (string("-t") == string(argv[i]) ) {
			test_mode = true;
			i += 1;
		}
		path = argv[i];
		melody = argv[i+1];
	} else {
		cout << "requires path searchfile " << endl;
		exit(1);
	}
	cout << "search " << melody << " for .cont in " << path << endl;

	unsigned int counter = 0;
	auto start = std::chrono::system_clock::now(); // 計測開始時刻
	if ( !test_mode ) {
		for (const fsys::directory_entry &entry : fsys::recursive_directory_iterator(path)) {
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
	} else {
		// 'path' をテキスト（被検索対象）にする
		int find = NaiveSearch(path, path.length(), melody, melody.length());
		if (find != -1) {
			cout << melody << ", " << find << endl;
		} else {
			cout << "no match" << endl;
		}
	}

	auto stop = std::chrono::system_clock::now(); 	// 計測終了時刻
	auto millisec = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count(); // ミリ秒に変換
	cout << "It took " << millisec << " milli seconds." << endl;

	return 0;
}
