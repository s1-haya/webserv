#include "color.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>
#include <stdexcept> // logic_error
#include <string>

namespace {

// 比較する型
typedef std::vector<std::string> Strs;

// expectedのStrsを作成
// templateのパラメータパックがc++11以降なので使っていない
// 手動で正しいsizeをセットする必要有り
Strs CreateExpect(const std::string *array, std::size_t size) {
	return Strs(array, array + size);
}

// SplitStr()を実行してexpectedと比較
int Run(const std::string &src, const std::string &substring, const Strs &expected) {
	static unsigned int test_case = 0;
	test_case++;

	if (utils::SplitStr(src, substring) == expected) {
		std::cerr << utils::color::GREEN << test_case << ".[OK] " << utils::color::RESET << src
				  << std::endl;
		return EXIT_SUCCESS;
	}
	std::cerr << utils::color::RED << test_case << ".[NG] " << utils::color::RESET << src
			  << std::endl;
	return EXIT_FAILURE;
}

} // namespace

int main() {
	int ret_code = 0;

	// ---------------------------------------------------------------------
	// test for SP(" "). substring:1文字
	// ---------------------------------------------------------------------
	// substringのみのstr
	const std::string expected_1[] = {"", ""};
	ret_code |= Run(" ", " ", CreateExpect(expected_1, 2));
	const std::string expected_2[] = {"", "", ""};
	ret_code |= Run("  ", " ", CreateExpect(expected_2, 3));

	// // substringがstrの中にない
	const std::string expected_3[] = {""};
	ret_code |= Run("", " ", CreateExpect(expected_3, 1));
	const std::string expected_4[] = {"abbccc"};
	ret_code |= Run("abbccc", " ", CreateExpect(expected_4, 1));

	// substringが1つずつ
	const std::string expected_5[] = {"a", "bb", "ccc"};
	ret_code |= Run("a bb ccc", " ", CreateExpect(expected_5, 3));
	// substringが連続
	const std::string expected_6[] = {"abb", "", "", "ccc"};
	ret_code |= Run("abb   ccc", " ", CreateExpect(expected_6, 4));

	// 先頭にsubstring
	const std::string expected_7[] = {"", "", "abbccc"};
	ret_code |= Run("  abbccc", " ", CreateExpect(expected_7, 3));
	// 末尾にsubstring
	const std::string expected_8[] = {"abbccc", ""};
	ret_code |= Run("abbccc ", " ", CreateExpect(expected_8, 2));
	// 前後にsubstring
	const std::string expected_9[] = {"", "abbccc", "", "", ""};
	ret_code |= Run(" abbccc   ", " ", CreateExpect(expected_9, 5));

	// ---------------------------------------------------------------------
	// test for CRLF("\r\n"). substring:2文字
	// ---------------------------------------------------------------------
	// substringのみのstr
	const std::string expected_10[] = {"", ""};
	ret_code |= Run("\r\n", "\r\n", CreateExpect(expected_10, 2));
	const std::string expected_11[] = {"", "", ""};
	ret_code |= Run("\r\n\r\n", "\r\n", CreateExpect(expected_11, 3));

	// substringがstrの中にない
	const std::string expected_12[] = {""};
	ret_code |= Run("", "\r\n", CreateExpect(expected_12, 1));
	const std::string expected_13[] = {"abbccc"};
	ret_code |= Run("abbccc", "\r\n", CreateExpect(expected_13, 1));

	// substringが1つずつ
	const std::string expected_14[] = {"a", "bb", "ccc"};
	ret_code |= Run("a\r\nbb\r\nccc", "\r\n", CreateExpect(expected_14, 3));
	// substringが連続
	const std::string expected_15[] = {"abb", "", "ccc"};
	ret_code |= Run("abb\r\n\r\nccc", "\r\n", CreateExpect(expected_15, 3));

	// 先頭にsubstring
	const std::string expected_16[] = {"", "abbccc"};
	ret_code |= Run("\r\nabbccc", "\r\n", CreateExpect(expected_16, 2));
	// 末尾にsubstring
	const std::string expected_17[] = {"abbccc", ""};
	ret_code |= Run("abbccc\r\n", "\r\n", CreateExpect(expected_17, 2));
	// 前後にsubstring
	const std::string expected_18[] = {"", "abbccc", "", ""};
	ret_code |= Run("\r\nabbccc\r\n\r\n", "\r\n", CreateExpect(expected_18, 4));

	// substringの中のcharがあっても何も起こらない
	const std::string expected_19[] = {"a\rbb", "ccc"};
	ret_code |= Run("a\rbb\r\nccc", "\r\n", CreateExpect(expected_19, 2));
	const std::string expected_20[] = {"a\nbb", "ccc"};
	ret_code |= Run("a\nbb\r\nccc", "\r\n", CreateExpect(expected_20, 2));

	// ---------------------------------------------------------------------
	// test for another case
	// ---------------------------------------------------------------------
	// substringが空文字列
	const std::string expected_21[] = {""};
	ret_code |= Run("", "", CreateExpect(expected_21, 1));
	const std::string expected_22[] = {"abc"};
	ret_code |= Run("abc", "", CreateExpect(expected_22, 1));

	return ret_code;
}
