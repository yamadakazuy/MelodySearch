// C++/Standard Template Library
// vector の使い方

// 標準入出力ライブラリのヘッダ、C の stdio.h に相当
#include <iostream>
// 標準 std:: ライブラリの vector コンテナを使うためのインクルード
#include <vector>
// C のライブラリもそのまま使える。strlen のためのインクルード
#include <string.h>

// 次の行の名前空間使用宣言を使うと、全ての std:: を省略できる
// using namespace std;

int main(int argc, char **argv) {
	// char の vector 配列（自動サイズ拡張）を宣言
	std::vector<char> pattern, text;
	// 初期サイズを指定する場合は std::vector<char> text(100); など

	// 第一引数
	if (argc != 3) {
		std::cerr << "パターン文字列、テキスト文字列をください。" << std::endl;
		// << は左（標準エラー出力 cerr）に右を引数で渡す演算子、
		// 右には文字、文字列、整数、浮動小数点数、論理値が使える。
		// std::endl は行末（改行）記号。
		// << 演算子の返し値は左辺なので連ねて記述可能。
		return -1;
	}
	for(int i = 0; i < strlen(argv[1]); ++i) {
		pattern.push_back(argv[1][i]); // 1文字ずつ最後に追加
	}

	for(int i = 0; i < strlen(argv[2]); ++i) {
		text.push_back(argv[2][i]); // 1文字ずつ最後に追加
	}

	std::cout << "naive search..." << std::endl;
	int i, j = 0;
	int pos;
	for(i = 0; i < text.size(); ++i) { // 長さは .size()
		if (j == pattern.size()) {
			pos = i - pattern.size();
			std::cout << "matched at " << pos << std::endl;
			break;
		}
		if (text[i] == pattern[j]) { // [添字] が使える。
			++j;
		} else {
			i -= j;
			j = 0;
		}
	}
	std::cout << std::endl;

	for(int i = 0; i < pos+pattern.size(); ++i) {
		std::cout << text[i];
	}
	std::cout << std::endl;
	return 0;
}
