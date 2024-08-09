#include <dirent.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

int main() {
	const char *path = "./";          // 読み込みたいディレクトリのパス
	DIR        *dir  = opendir(path); // ディレクトリを開く

	if (dir == nullptr) {
		std::cerr << "Error opening directory" << std::endl;
		return 1;
	}

	struct dirent *entry;
	while ((entry = readdir(dir)) != nullptr) {  // ディレクトリの内容を読み込む
		std::cout << entry->d_name << std::endl; // ファイル名を表示
	}

	if (access("main.cpp", F_OK) == 0)
		std::cout << "main.cpp exists" << std::endl;

	std::string a = "/content/www/index";
	std::string b = "/content/www/";
	std::string c = "/";

	std::cout << a.find_first_not_of(b) << std::endl;
	std::cout << a.find_first_not_of(c) << std::endl;

	closedir(dir); // ディレクトリを閉じる

	std::string str         = "Hello, world!";
	std::string toReplace   = "world";
	std::string replacement = "C++";

	// 置換する部分文字列の開始位置を見つける
	std::string::size_type startPos = str.find(toReplace);
	if (startPos != std::string::npos) {
		// replace() 関数を使って置換する
		str.replace(startPos, toReplace.length(), replacement);
		std::cout << "After replacement: " << str << std::endl;
	} else {
		std::cout << "'" << toReplace << "' not found in the string." << std::endl;
	}

	return 0;
}
