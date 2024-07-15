#include "utils.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

namespace test_split {
namespace {

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// 比較する型
typedef std::vector<std::string> Strs;

// expected
// templateのパラメータパックがc++11以降なので使っていない
struct TestCase {
	TestCase(
		const std::string &tmp_input,
		const std::string &tmp_substring,
		const std::string *array,
		std::size_t        size
	)
		: input(tmp_input), substring(tmp_substring), expected(array, array + size) {}
	const std::string input;
	const std::string substring;
	const Strs        expected;
};

// SplitStr()を実行してexpectedと比較
int Run(
	std::size_t        test_case_num,
	const std::string &src,
	const std::string &substring,
	const Strs        &expected
) {
	if (utils::SplitStr(src, substring) == expected) {
		std::cout << utils::color::GREEN << test_case_num << ".[OK] " << utils::color::RESET
				  << std::endl;
		return EXIT_SUCCESS;
	}
	std::cerr << utils::color::RED << test_case_num << ".[NG] " << utils::color::RESET << std::endl;
	std::cerr << utils::color::RED << "SplitStr() failed" << utils::color::RESET << std::endl;
	std::cerr << "src: [" << src << "]" << std::endl;
	return EXIT_FAILURE;
}

int RunTestCases(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = 0;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		ret_code |= Run(i + 1, test_case.input, test_case.substring, test_case.expected);
	}
	return ret_code;
}

} // namespace

int RunTest() {
	int ret_code = 0;

	// ---------------------------------------------------------------------
	// test for SP(" "). substring:1文字
	// ---------------------------------------------------------------------
	static const std::string expected_1[] = {"", ""};
	static const std::string expected_2[] = {"", "", ""};
	static const std::string expected_3[] = {""};
	static const std::string expected_4[] = {"abbccc"};
	static const std::string expected_5[] = {"a", "bb", "ccc"};
	static const std::string expected_6[] = {"abb", "", "", "ccc"};
	static const std::string expected_7[] = {"", "", "abbccc"};
	static const std::string expected_8[] = {"abbccc", ""};
	static const std::string expected_9[] = {"", "abbccc", "", "", ""};

	static const TestCase test_cases_for_space[] = {

		// substringのみのstr
		TestCase(" ", " ", expected_1, ARRAY_SIZE(expected_1)),
		TestCase("  ", " ", expected_2, ARRAY_SIZE(expected_2)),
		// substringがstrの中にない
		TestCase("", " ", expected_3, ARRAY_SIZE(expected_3)),
		TestCase("abbccc", " ", expected_4, ARRAY_SIZE(expected_4)),
		// substringが1つずつ
		TestCase("a bb ccc", " ", expected_5, ARRAY_SIZE(expected_5)),
		// substringが連続
		TestCase("abb   ccc", " ", expected_6, ARRAY_SIZE(expected_6)),
		// 先頭にsubstring
		TestCase("  abbccc", " ", expected_7, ARRAY_SIZE(expected_7)),
		// 末尾にsubstring
		TestCase("abbccc ", " ", expected_8, ARRAY_SIZE(expected_8)),
		// 前後にsubstring
		TestCase(" abbccc   ", " ", expected_9, ARRAY_SIZE(expected_9))
	};

	ret_code |= RunTestCases(test_cases_for_space, ARRAY_SIZE(test_cases_for_space));

	// ---------------------------------------------------------------------
	// test for CRLF("\r\n"). substring:2文字
	// ---------------------------------------------------------------------
	static const std::string expected_10[] = {"", ""};
	static const std::string expected_11[] = {"", "", ""};
	static const std::string expected_12[] = {""};
	static const std::string expected_13[] = {"abbccc"};
	static const std::string expected_14[] = {"a", "bb", "ccc"};
	static const std::string expected_15[] = {"abb", "", "ccc"};
	static const std::string expected_16[] = {"", "abbccc"};
	static const std::string expected_17[] = {"abbccc", ""};
	static const std::string expected_18[] = {"", "abbccc", "", ""};
	static const std::string expected_19[] = {"a\rbb", "ccc"};
	static const std::string expected_20[] = {"a\nbb", "ccc"};

	static const TestCase test_cases_for_crlf[] = {

		// substringのみのstr
		TestCase("\r\n", "\r\n", expected_10, ARRAY_SIZE(expected_10)),
		TestCase("\r\n\r\n", "\r\n", expected_11, ARRAY_SIZE(expected_11)),
		// substringがstrの中にない
		TestCase("", "\r\n", expected_12, ARRAY_SIZE(expected_12)),
		TestCase("abbccc", "\r\n", expected_13, ARRAY_SIZE(expected_13)),
		// substringが1つずつ
		TestCase("a\r\nbb\r\nccc", "\r\n", expected_14, ARRAY_SIZE(expected_14)),
		// substringが連続
		TestCase("abb\r\n\r\nccc", "\r\n", expected_15, ARRAY_SIZE(expected_15)),
		// 先頭にsubstring
		TestCase("\r\nabbccc", "\r\n", expected_16, ARRAY_SIZE(expected_16)),
		// 末尾にsubstring
		TestCase("abbccc\r\n", "\r\n", expected_17, ARRAY_SIZE(expected_17)),
		// 前後にsubstring
		TestCase("\r\nabbccc\r\n\r\n", "\r\n", expected_18, ARRAY_SIZE(expected_18)),
		// substringの中のcharがあっても何も起こらない
		TestCase("a\rbb\r\nccc", "\r\n", expected_19, ARRAY_SIZE(expected_19)),
		TestCase("a\nbb\r\nccc", "\r\n", expected_20, ARRAY_SIZE(expected_20))
	};

	ret_code |= RunTestCases(test_cases_for_crlf, ARRAY_SIZE(test_cases_for_crlf));

	// ---------------------------------------------------------------------
	// test for another case
	// ---------------------------------------------------------------------
	static const std::string expected_21[] = {""};
	static const std::string expected_22[] = {"abc"};

	static const TestCase test_cases_for_other[] = {

		// substringが空文字列
		TestCase("", "", expected_21, ARRAY_SIZE(expected_21)),
		TestCase("abc", "", expected_22, ARRAY_SIZE(expected_22))
	};

	ret_code |= RunTestCases(test_cases_for_other, ARRAY_SIZE(test_cases_for_other));

	return ret_code;
}

} // namespace test_split
