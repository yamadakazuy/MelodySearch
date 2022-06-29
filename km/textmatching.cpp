#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <windows.h>
#include <vector>

class Dir {
public:
	// コンストラクタ
	Dir(void) {
	}
	// デストラクタ
	virtual ~Dir(void) {
	}

	// ファイル一覧取得
	// folder : フォルダの絶対パスを入力とする
	// 例 : "D:\\Users\\Pictures\\"
	static std::vector<std::string> read(std::string folder) {
		// 宣言
		std::vector<std::string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// ファイル名検索のためにワイルドカード追加
		// 例 : "D:\\Users\\Pictures\\*.*"
		std::stringstream ss;
		ss << folder;
		std::string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\')
			ss << '\\';
		ss << "*.*";

		// ファイル探索
		// FindFirstFile(ファイル名, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// 検索失敗
		if (hFind == INVALID_HANDLE_VALUE) {
			std::cout << "ファイル一覧を取得できませんでした" << std::endl;
			exit(1); // エラー終了
		}

		// ファイル名をリストに格納するためのループ
		do {
			// フォルダは除く
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
					&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
				//ファイル名をリストに格納
				char *file = fd.cFileName;
				std::string str = file;
				fileList.push_back(str);
			}
		} while (FindNextFile(hFind, &fd)); //次のファイルを探索

		// hFindのクローズ
		FindClose(hFind);

		return fileList;
	}
};

int strmatch(std::string text, std::string pattern, int start = 0) {
	unsigned int pos, ix;
	for(pos = start; pos < text.length(); ++pos) {
		for(ix = 0; ix < pattern.length()
		and pos + ix < text.length() and text[pos+ix] == pattern[ix]; ++ix) { }
		if ( ix == pattern.length() )
			return pos;
	}
	return -1;
}

int main(int argc, char **argv) {
	// 開始時間
	auto start = std::chrono::high_resolution_clock::now();

	if (argc <= 1) {
		printf("C-style print to standard out by stdio.h\n");
		printf("usage: command pattern_string filename [return]\n");
		return 1;
	} else {
		std::cout << "C++-style print to standard out by iostream" << std::endl;
		std::cout << "pattern = \"" << argv[1] << std::endl;
	}

    std::string pattern(argv[1]);
	std::ifstream file;
    std::string line;

    //for (const auto & file : directory_iterator(path))
    //   cout << file.path() << endl;
    // ファイル入力
	std::vector<std::string> backfilelist = Dir::read("..\\smf");
	for(unsigned int i = 0; i < backfilelist.size(); i++){
        int pos;
        std::cout << backfilelist[i] << std::endl;
        file.open(backfilelist[i]);
        while (std::getline(file, line)){
            pos = strmatch(backfilelist[i], pattern);
            std::cout << pos << std::endl;
            if (pos >= 0){
                std::cout << backfilelist[i] << std::endl;
            }
        }
        file.close();
	}

	// 終了時間
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);



	std::cout << "elapsed_time = " << elapsed.count() * 1e-9 << "sec.\n" << std::endl;

	return 0;
}
