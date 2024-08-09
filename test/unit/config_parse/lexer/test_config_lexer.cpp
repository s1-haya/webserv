#include "color.hpp"
#include "lexer.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace { /*テスト汎用*/

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

struct Result {
	Result() : is_success(true) {}
	bool        is_success;
	std::string error_log;
};

int GetTestCaseNum() {
	static unsigned int test_case_num = 0;
	++test_case_num;
	return test_case_num;
}

void PrintOk() {
	std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK] " << utils::color::RESET
			  << std::endl;
}

void PrintNg() {
	std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
			  << std::endl;
}

void PrintError(const std::string &message) {
	std::cerr << utils::color::RED << message << utils::color::RESET << std::endl;
}

} // namespace

/*------------------------------------------------------------------*/
/*------------------------------------------------------------------*/

/* namespace内で定義しないと他のコードで使えない */
namespace config {
namespace node {

bool operator==(const node::Node &lhs, const node::Node &rhs) {
	return lhs.token == rhs.token && lhs.token_type == rhs.token_type;
}

bool operator!=(const node::Node &lhs, const node::Node &rhs) {
	return lhs.token != rhs.token || lhs.token_type != rhs.token_type;
}

} // namespace node
} // namespace config

using namespace config;

namespace { /*lexer固有*/

typedef std::list<node::Node> NodeList;

// expected
struct TestCase {
	TestCase(const std::string &input, const NodeList &expected)
		: input(input), expected(expected) {}
	const std::string input;
	const NodeList    expected;
};

std::ostream &operator<<(std::ostream &os, const node::Node node) {
	os << "{token: " << node.token << ", "
	   << "token_type: " << node.token_type << "}";
	return os;
}

std::ostream &operator<<(std::ostream &os, const NodeList &nodes) {
	NodeList::const_iterator it = nodes.begin();
	while (it != nodes.end()) {
		os << *it;
		++it;
		if (it != nodes.end()) {
			os << ", ";
		}
	}
	return os;
}

Result Run(const std::string &src, const NodeList &expected) {
	Result run_result;

	NodeList     nodes;
	lexer::Lexer lex(src, nodes);
	if (nodes != expected) {
		std::ostringstream error_log;
		error_log << "- Expected: [ " << expected << " ]\n";
		error_log << "- Result  : [ " << nodes << " ]\n";
		run_result.is_success = false;
		run_result.error_log  = error_log.str();
	}
	return run_result;
}

int Test(const Result &result, const std::string &src) {
	if (result.is_success) {
		PrintOk();
		return EXIT_SUCCESS;
	} else {
		PrintNg();
		PrintError("ConfigLexer failed:");
		std::cerr << "src:[\n" << src << "]" << std::endl;
		std::cerr << "--------------------------------" << std::endl;
		std::cerr << result.error_log;
		return EXIT_FAILURE;
	}
}

int RunTests(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = 0;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		ret_code |= Test(Run(test_case.input, test_case.expected), test_case.input);
	}
	return ret_code;
}

/* Test1 One Server*/
NodeList MakeExpectedTest1() {
	NodeList expected_result;

	node::Node expected_node_1 = {"server", node::CONTEXT};
	node::Node expected_node_2 = {"{", node::L_BRACKET};
	node::Node expected_node_3 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);

	return expected_result;
}

/* Test2 One Server One Location*/
NodeList MakeExpectedTest2() {
	NodeList expected_result;

	node::Node expected_node_1 = {"server", node::CONTEXT};
	node::Node expected_node_2 = {"{", node::L_BRACKET};
	node::Node expected_node_3 = {"location", node::CONTEXT};
	node::Node expected_node_4 = {"/", node::WORD};
	node::Node expected_node_5 = {"{", node::L_BRACKET};
	node::Node expected_node_6 = {"}", node::R_BRACKET};
	node::Node expected_node_7 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);
	expected_result.push_back(expected_node_4);
	expected_result.push_back(expected_node_5);
	expected_result.push_back(expected_node_6);
	expected_result.push_back(expected_node_7);

	return expected_result;
}

/* Test3 listen, server_name, alias, index Directive*/
NodeList MakeExpectedTest3() {
	NodeList expected_result;

	node::Node expected_node_1  = {"server", node::CONTEXT};
	node::Node expected_node_2  = {"{", node::L_BRACKET};
	node::Node expected_node_3  = {"listen", node::DIRECTIVE};
	node::Node expected_node_4  = {"8080", node::WORD};
	node::Node expected_node_5  = {";", node::DELIM};
	node::Node expected_node_6  = {"server_name", node::DIRECTIVE};
	node::Node expected_node_7  = {"localhost", node::WORD};
	node::Node expected_node_8  = {";", node::DELIM};
	node::Node expected_node_9  = {"location", node::CONTEXT};
	node::Node expected_node_10 = {"/", node::WORD};
	node::Node expected_node_11 = {"{", node::L_BRACKET};
	node::Node expected_node_12 = {"alias", node::DIRECTIVE};
	node::Node expected_node_13 = {"/data/", node::WORD};
	node::Node expected_node_14 = {";", node::DELIM};
	node::Node expected_node_15 = {"index", node::DIRECTIVE};
	node::Node expected_node_16 = {"index.html", node::WORD};
	node::Node expected_node_17 = {";", node::DELIM};
	node::Node expected_node_18 = {"}", node::R_BRACKET};
	node::Node expected_node_19 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);
	expected_result.push_back(expected_node_4);
	expected_result.push_back(expected_node_5);
	expected_result.push_back(expected_node_6);
	expected_result.push_back(expected_node_7);
	expected_result.push_back(expected_node_8);
	expected_result.push_back(expected_node_9);
	expected_result.push_back(expected_node_10);
	expected_result.push_back(expected_node_11);
	expected_result.push_back(expected_node_12);
	expected_result.push_back(expected_node_13);
	expected_result.push_back(expected_node_14);
	expected_result.push_back(expected_node_15);
	expected_result.push_back(expected_node_16);
	expected_result.push_back(expected_node_17);
	expected_result.push_back(expected_node_18);
	expected_result.push_back(expected_node_19);

	return expected_result;
}

/* Test4 Multiple ports, "server_name" as server_name*/
NodeList MakeExpectedTest4() {
	NodeList expected_result;

	node::Node expected_node_1  = {"server", node::CONTEXT};
	node::Node expected_node_2  = {"{", node::L_BRACKET};
	node::Node expected_node_3  = {"listen", node::DIRECTIVE};
	node::Node expected_node_4  = {"8080", node::WORD};
	node::Node expected_node_5  = {"8000", node::WORD};
	node::Node expected_node_6  = {"80", node::WORD};
	node::Node expected_node_7  = {";", node::DELIM};
	node::Node expected_node_8  = {"server_name", node::DIRECTIVE};
	node::Node expected_node_9  = {"server_name", node::WORD};
	node::Node expected_node_10 = {";", node::DELIM};
	node::Node expected_node_11 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);
	expected_result.push_back(expected_node_4);
	expected_result.push_back(expected_node_5);
	expected_result.push_back(expected_node_6);
	expected_result.push_back(expected_node_7);
	expected_result.push_back(expected_node_8);
	expected_result.push_back(expected_node_9);
	expected_result.push_back(expected_node_10);
	expected_result.push_back(expected_node_11);

	return expected_result;
}

/* Test5 No Delimiter*/
NodeList MakeExpectedTest5() {
	NodeList expected_result;

	node::Node expected_node_1  = {"server", node::CONTEXT};
	node::Node expected_node_2  = {"{", node::L_BRACKET};
	node::Node expected_node_3  = {"listen", node::DIRECTIVE};
	node::Node expected_node_4  = {"8080", node::WORD};
	node::Node expected_node_5  = {"8000", node::WORD};
	node::Node expected_node_6  = {"80", node::WORD};
	node::Node expected_node_7  = {"server_name", node::WORD};
	node::Node expected_node_8  = {"localhost", node::WORD};
	node::Node expected_node_9  = {";", node::DELIM};
	node::Node expected_node_10 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);
	expected_result.push_back(expected_node_4);
	expected_result.push_back(expected_node_5);
	expected_result.push_back(expected_node_6);
	expected_result.push_back(expected_node_7);
	expected_result.push_back(expected_node_8);
	expected_result.push_back(expected_node_9);
	expected_result.push_back(expected_node_10);

	return expected_result;
}

/* Test6 Multiple Locations*/
NodeList MakeExpectedTest6() {
	NodeList expected_result;

	node::Node expected_node_1  = {"server", node::CONTEXT};
	node::Node expected_node_2  = {"{", node::L_BRACKET};
	node::Node expected_node_3  = {"server_name", node::DIRECTIVE};
	node::Node expected_node_4  = {"test.serv", node::WORD};
	node::Node expected_node_5  = {";", node::DELIM};
	node::Node expected_node_6  = {"location", node::CONTEXT};
	node::Node expected_node_7  = {"/", node::WORD};
	node::Node expected_node_8  = {"{", node::L_BRACKET};
	node::Node expected_node_9  = {"index", node::DIRECTIVE};
	node::Node expected_node_10 = {"index.html", node::WORD};
	node::Node expected_node_11 = {";", node::DELIM};
	node::Node expected_node_12 = {"}", node::R_BRACKET};
	node::Node expected_node_13 = {"location", node::CONTEXT};
	node::Node expected_node_14 = {"/www/", node::WORD};
	node::Node expected_node_15 = {"{", node::L_BRACKET};
	node::Node expected_node_16 = {"index", node::DIRECTIVE};
	node::Node expected_node_17 = {"index", node::WORD};
	node::Node expected_node_18 = {";", node::DELIM};
	node::Node expected_node_19 = {"}", node::R_BRACKET};
	node::Node expected_node_20 = {"}", node::R_BRACKET};

	expected_result.push_back(expected_node_1);
	expected_result.push_back(expected_node_2);
	expected_result.push_back(expected_node_3);
	expected_result.push_back(expected_node_4);
	expected_result.push_back(expected_node_5);
	expected_result.push_back(expected_node_6);
	expected_result.push_back(expected_node_7);
	expected_result.push_back(expected_node_8);
	expected_result.push_back(expected_node_9);
	expected_result.push_back(expected_node_10);
	expected_result.push_back(expected_node_11);
	expected_result.push_back(expected_node_12);
	expected_result.push_back(expected_node_13);
	expected_result.push_back(expected_node_14);
	expected_result.push_back(expected_node_15);
	expected_result.push_back(expected_node_16);
	expected_result.push_back(expected_node_17);
	expected_result.push_back(expected_node_18);
	expected_result.push_back(expected_node_19);
	expected_result.push_back(expected_node_20);

	return expected_result;
}

/* Test7 Quotation*/
// NodeList MakeExpectedTest7() {
// 	NodeList expected_result;

// 	node::Node expected_node_1  = {"server", node::CONTEXT};
// 	node::Node expected_node_2  = {"{", node::L_BRACKET};
// 	node::Node expected_node_3  = {"server_name", node::DIRECTIVE};
// 	node::Node expected_node_4  = {"test.serv", node::WORD};
// 	node::Node expected_node_5  = {";", node::DELIM};
// 	node::Node expected_node_6  = {"location", node::CONTEXT};
// 	node::Node expected_node_7  = {"/", node::WORD};
// 	node::Node expected_node_8  = {"{", node::L_BRACKET};
// 	node::Node expected_node_9  = {"index", node::DIRECTIVE};
// 	node::Node expected_node_10 = {"index.html", node::WORD};
// 	node::Node expected_node_11 = {";", node::DELIM};
// 	node::Node expected_node_12 = {"}", node::R_BRACKET};
// 	node::Node expected_node_13 = {"}", node::R_BRACKET};

// 	expected_result.push_back(expected_node_1);
// 	expected_result.push_back(expected_node_2);
// 	expected_result.push_back(expected_node_3);
// 	expected_result.push_back(expected_node_4);
// 	expected_result.push_back(expected_node_5);
// 	expected_result.push_back(expected_node_6);
// 	expected_result.push_back(expected_node_7);
// 	expected_result.push_back(expected_node_8);
// 	expected_result.push_back(expected_node_9);
// 	expected_result.push_back(expected_node_10);
// 	expected_result.push_back(expected_node_11);
// 	expected_result.push_back(expected_node_12);
// 	expected_result.push_back(expected_node_13);

// 	return expected_result;
// }

} // namespace

int main() {
	int ret_code = EXIT_SUCCESS;

	NodeList expected_result_test_1 = MakeExpectedTest1();
	NodeList expected_result_test_2 = MakeExpectedTest2();
	NodeList expected_result_test_3 = MakeExpectedTest3();
	NodeList expected_result_test_4 = MakeExpectedTest4();
	NodeList expected_result_test_5 = MakeExpectedTest5();
	NodeList expected_result_test_6 = MakeExpectedTest6();
	// NodeList expected_result_test_7 = MakeExpectedTest7();

	static TestCase test_cases[] = {
		TestCase(
			"server {\n \
				}\n",
			expected_result_test_1
		),
		TestCase(
			"server {\n \
					location / {\n \
					}\n \
				}\n",
			expected_result_test_2
		),
		TestCase(
			"server {\n \
					listen 8080;\n \
					server_name localhost;\n \
					location / {\n \
						alias /data/;\n \
						index index.html;\n \
					}\n \
				}\n",
			expected_result_test_3
		),
		TestCase(
			"server {\n \
					listen 8080 8000 80;\n \
					server_name server_name;\n \
				}\n",
			expected_result_test_4
		),
		TestCase(
			"server {\n \
					listen 8080 8000 80\n \
					server_name localhost;\n \
				}\n",
			expected_result_test_5
		),
		TestCase(
			"server {\n \
					server_name test.serv;\n \
					location / {\n \
						index index.html;\n \
					}\n \
					location /www/ {\n \
						index index;\n \
					}\n \
				}\n",
			expected_result_test_6
		),
		// TODO: \を追加
		// TestCase(
		// 	"server {\n
		// 			server_name \"test.serv\";\n
		// 			location / {\n
		// 				index \"index.html\";\n
		// 			}\n
		// 		}\n",
		// 	expected_result_test_7
		// ),
	};

	ret_code |= RunTests(test_cases, ARRAY_SIZE(test_cases));

	return ret_code;
}
