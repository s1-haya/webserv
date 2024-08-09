#include "color.hpp"
#include "context.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"
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
namespace context {

bool operator==(const LocationCon &lhs, const LocationCon &rhs) {
	return lhs.request_uri == rhs.request_uri && lhs.alias == rhs.alias && lhs.index == rhs.index &&
		   lhs.autoindex == rhs.autoindex && lhs.allowed_methods == rhs.allowed_methods &&
		   lhs.redirect == rhs.redirect;
}

bool operator!=(const LocationCon &lhs, const LocationCon &rhs) {
	return lhs.request_uri != rhs.request_uri || lhs.alias != rhs.alias || lhs.index != rhs.index ||
		   lhs.autoindex != rhs.autoindex || lhs.allowed_methods != rhs.allowed_methods ||
		   lhs.redirect != rhs.redirect;
}

bool operator==(const ServerCon &lhs, const ServerCon &rhs) {
	return lhs.port == rhs.port && lhs.server_names == rhs.server_names &&
		   lhs.location_con == rhs.location_con &&
		   lhs.client_max_body_size == rhs.client_max_body_size && lhs.error_page == rhs.error_page;
}

bool operator!=(const ServerCon &lhs, const ServerCon &rhs) {
	return lhs.port != rhs.port || lhs.server_names != rhs.server_names ||
		   lhs.location_con != rhs.location_con ||
		   lhs.client_max_body_size != rhs.client_max_body_size || lhs.error_page != rhs.error_page;
}

} // namespace context
} // namespace config

using namespace config;

namespace { /*parser固有*/

typedef std::list<node::Node>           NodeList;
typedef std::list<context::LocationCon> LocationList;
typedef std::list<context::ServerCon>   ServerList;

// expected
struct TestCase {
	TestCase(const std::string &input, const ServerList &expected)
		: input(input), expected(expected) {}
	const std::string input;
	const ServerList  expected;
};

/* Factory Functions */
// 引数付きのFactory Function: 引数ありコンストラクタはここでしか使わないため
context::LocationCon BuildLocationCon(
	const std::string                          &request_uri,
	const std::string                          &alias,
	const std::string                          &index,
	bool                                        autoindex,
	const std::list<std::string>               &allowed_methods,
	const std::pair<unsigned int, std::string> &redirect
) {
	context::LocationCon loc;
	loc.request_uri     = request_uri;
	loc.alias           = alias;
	loc.index           = index;
	loc.autoindex       = autoindex;
	loc.allowed_methods = allowed_methods;
	loc.redirect        = redirect;
	return loc;
}

// 引数付きのFactory Function: 引数ありコンストラクタはここでしか使わないため
context::ServerCon BuildServerCon(
	const std::list<int>                       &port,
	const std::list<std::string>               &server_names,
	const LocationList                         &location_con,
	std::size_t                                 client_max_body_size,
	const std::pair<unsigned int, std::string> &error_page
) {
	context::ServerCon server;
	server.port                 = port;
	server.server_names         = server_names;
	server.location_con         = location_con;
	server.client_max_body_size = client_max_body_size;
	server.error_page           = error_page;
	return server;
}

/* For server_names, allowed_methods */
std::ostream &operator<<(std::ostream &os, const std::list<std::string> &str_list) {
	std::list<std::string>::const_iterator it = str_list.begin();
	while (it != str_list.end()) {
		os << *it;
		++it;
		if (it != str_list.end()) {
			os << ", ";
		}
	}
	return os;
}

/* For ports */
std::ostream &operator<<(std::ostream &os, const std::list<int> &int_list) {
	std::list<int>::const_iterator it = int_list.begin();
	while (it != int_list.end()) {
		os << *it;
		++it;
		if (it != int_list.end()) {
			os << ", ";
		}
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, const context::LocationCon &location) {
	os << "{location: " << location.request_uri << ", "
	   << "alias: " << location.alias << ", "
	   << "index: " << location.index << ", "
	   << "autoindex: " << (location.autoindex ? "true" : "false") << ", "
	   << "allowed_method: " << location.allowed_methods << ", "
	   << "return(status): " << location.redirect.first << ", "
	   << "return(index): " << location.redirect.second << "}";
	return os;
}

std::ostream &operator<<(std::ostream &os, const LocationList &locations) {
	LocationList::const_iterator it = locations.begin();
	while (it != locations.end()) {
		os << *it;
		++it;
		if (it != locations.end()) {
			os << ", ";
		}
	}
	return os;
}

std::ostream &operator<<(std::ostream &os, const context::ServerCon &server) {
	os << "{port: " << server.port << ", "
	   << "server_name: " << server.server_names << ", "
	   << "location_context: " << server.location_con << ", "
	   << "client_max_body_size: " << server.client_max_body_size << ", "
	   << "error_page(status): " << server.error_page.first << ", "
	   << "error_page(index): " << server.error_page.second << "}";
	return os;
}

std::ostream &operator<<(std::ostream &os, const ServerList &servers) {
	ServerList::const_iterator it = servers.begin();
	while (it != servers.end()) {
		os << *it;
		++it;
		if (it != servers.end()) {
			os << ", ";
		}
	}
	return os;
}

Result Run(const std::string &src, const ServerList &expected) {
	Result run_result;

	NodeList       nodes;
	lexer::Lexer   lex(src, nodes);
	parser::Parser par(nodes);
	if (par.GetServers() != expected) {
		std::ostringstream error_log;
		error_log << "- Expected: [ " << expected << " ]\n";
		error_log << "- Result  : [ " << par.GetServers() << " ]\n";
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
		PrintError("ConfigParser failed:");
		std::cerr << "src:[\n" << src << "]" << std::endl;
		std::cerr << "--------------------------------" << std::endl;
		std::cerr << result.error_log;
		return EXIT_FAILURE;
	}
}

int RunTests(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = EXIT_SUCCESS;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		ret_code |= Test(Run(test_case.input, test_case.expected), test_case.input);
	}
	return ret_code;
}

/* For Error Tests */
int RunErrorTests(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = EXIT_SUCCESS;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		try {
			Result result = Run(test_case.input, test_case.expected);
			PrintNg();
			PrintError("ConfigParser failed (No Throw):");
			std::cerr << "src:[\n" << test_case.input << "]" << std::endl;
			ret_code |= EXIT_FAILURE;
		} catch (const std::exception &e) {
			PrintOk();
			utils::Debug(e.what());
		}
	}
	return ret_code;
}

// TODO: Lexerとテストケースを揃える

/* Test1 One Server */
ServerList MakeExpectedTest1() {
	ServerList                           expected_result;
	std::list<int>                       expected_ports_1;
	std::list<std::string>               server_names_1;
	LocationList                         expected_locationlist_1;
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test2 One Server, One Location */
ServerList MakeExpectedTest2() {
	ServerList                           expected_result;
	std::list<int>                       expected_ports_1;
	std::list<std::string>               server_names_1;
	LocationList                         expected_locationlist_1;
	std::list<std::string>               allowed_methods_1;
	std::pair<unsigned int, std::string> redirect_1;
	context::LocationCon                 expected_location_1_1 =
		BuildLocationCon("/", "", "", false, allowed_methods_1, redirect_1);
	expected_locationlist_1.push_back(expected_location_1_1);
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test3 Server, Location Directive */
ServerList MakeExpectedTest3() {
	ServerList     expected_result;
	std::list<int> expected_ports_1;
	expected_ports_1.push_back(8080);
	std::list<std::string> server_names_1;
	server_names_1.push_back("localhost");
	LocationList                         expected_locationlist_1;
	std::list<std::string>               allowed_methods_1;
	std::pair<unsigned int, std::string> redirect_1;
	context::LocationCon                 expected_location_1_1 =
		BuildLocationCon("/", "/data/", "index.html", false, allowed_methods_1, redirect_1);
	expected_locationlist_1.push_back(expected_location_1_1);
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test4 Multiple ports */
ServerList MakeExpectedTest4() {
	ServerList             expected_result;
	std::list<std::string> server_names_1;
	server_names_1.push_back("server_name");
	server_names_1.push_back("server");
	std::list<int> expected_ports_1;
	expected_ports_1.push_back(8080);
	expected_ports_1.push_back(8000);
	expected_ports_1.push_back(80);
	LocationList                         expected_locationlist_1;
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test5 Multiple Locations */
ServerList MakeExpectedTest5() {
	ServerList             expected_result;
	std::list<int>         expected_ports_1;
	std::list<std::string> server_names_1;
	server_names_1.push_back("test.serv");
	LocationList                         expected_locationlist_1;
	std::list<std::string>               allowed_methods_1_1;
	std::pair<unsigned int, std::string> redirect_1_1;
	context::LocationCon                 expected_location_1_1 =
		BuildLocationCon("/", "", "index.html", false, allowed_methods_1_1, redirect_1_1);
	std::list<std::string>               allowed_methods_1_2;
	std::pair<unsigned int, std::string> redirect_1_2;
	context::LocationCon                 expected_location_1_2 =
		BuildLocationCon("/www/", "", "index", false, allowed_methods_1_2, redirect_1_2);
	expected_locationlist_1.push_back(expected_location_1_1);
	expected_locationlist_1.push_back(expected_location_1_2);
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test6 Tab+Space, Comment */
ServerList MakeExpectedTest6() {
	ServerList             expected_result;
	std::list<std::string> server_names_1;
	server_names_1.push_back("comment.serv");
	std::list<int>                       expected_ports_1;
	LocationList                         expected_locationlist_1;
	std::list<std::string>               allowed_methods_1;
	std::pair<unsigned int, std::string> redirect_1;
	context::LocationCon                 expected_location_1_1 =
		BuildLocationCon("/", "", "index.html", false, allowed_methods_1, redirect_1);
	expected_locationlist_1.push_back(expected_location_1_1);
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

/* Test7 Multiple Servers */
ServerList MakeExpectedTest7() {
	ServerList expected_result;

	std::list<int> expected_ports_1;
	expected_ports_1.push_back(8080);
	std::list<std::string> server_names_1;
	server_names_1.push_back("localhost");
	LocationList                         expected_locationlist_1;
	std::pair<unsigned int, std::string> error_page_1;
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 1024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	std::list<int> expected_ports_2;
	expected_ports_2.push_back(12345);
	std::list<std::string> server_names_2;
	server_names_2.push_back("test.www");
	LocationList                         expected_locationlist_2;
	std::pair<unsigned int, std::string> error_page_2;
	context::ServerCon                   expected_server_2 = BuildServerCon(
        expected_ports_2, server_names_2, expected_locationlist_2, 1024, error_page_2
    );
	expected_result.push_back(expected_server_2);

	return expected_result;
}

} // namespace

int main() {
	int ret_code = EXIT_SUCCESS;

	ServerList expected_result_test_1 = MakeExpectedTest1();
	ServerList expected_result_test_2 = MakeExpectedTest2();
	ServerList expected_result_test_3 = MakeExpectedTest3();
	ServerList expected_result_test_4 = MakeExpectedTest4();
	ServerList expected_result_test_5 = MakeExpectedTest5();
	ServerList expected_result_test_6 = MakeExpectedTest6();
	ServerList expected_result_test_7 = MakeExpectedTest7();

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
					listen 8080;\n \
					listen 8000;\n \
					listen 80;\n \
					server_name server_name server;\n \
				}\n",
			expected_result_test_4
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
			expected_result_test_5
		),
		TestCase(
			"server {\n \
					\n \
				    \n \
			#ashjkahsk\n\
					server_name comment.serv;\n \
					location / {\n \
						index index.html;\n \
				    #ashjkahsk\n \
					}\n \
				}\n",
			expected_result_test_6
		),
		TestCase(
			"server {\n \
					listen 8080;\n \
					server_name localhost;\n \
				}\n \
			 server {\n \
					listen 12345;\n \
					server_name test.www;\n \
				}\n",
			expected_result_test_7
		)
	};

	ret_code |= RunTests(test_cases, ARRAY_SIZE(test_cases));

	ServerList expected_result_error_test;

	static TestCase error_test_cases[] = {
		/* Test8 */ TestCase(
			"server {\n \
				\n",
			expected_result_error_test
		),
		/* Test9 */
		TestCase(
			"server {\n \
					server {\n \
					}\n \
				}\n",
			expected_result_error_test
		),
		/* Test10 */
		TestCase(
			"server {\n \
					listen\n \
				}\n",
			expected_result_error_test
		),
		/* Test11 */
		TestCase(
			"server {\n \
					server_name server_name\n \
				}\n",
			expected_result_error_test
		),
		/* Test12 */
		// TestCase(
		// 	"server {\n
		// 			listen 8080\n
		// 			server_name localhost;\n
		// 		}\n",
		// 	expected_result_error_test
		// ),
		/* Test13 */
		// TestCase(
		// 	"server {\n
		// 			listen 8080 8000;\n
		// 			server_name localhost;\n
		// 		}\n",
		// 	expected_result_error_test
		// ),
		/* Test14 */
		// TestCase(
		// 	"serv {\n
		// 		}\n",
		// 	expected_result_error_test
		// ),
		/* Test15 */
		TestCase(
			"server {\n \
					location / /www/ {\n \
					}\n \
				}\n",
			expected_result_error_test
		),
		/* Test16 */
		TestCase(
			"server {\n \
					location /www/ {\n \
					\n \
				}\n",
			expected_result_error_test
		),
		/* Test17 */
		TestCase(
			"server {\n \
					listen 8080;\n \
					server_name localhost;\n \
					location / {\n \
						alias\n \
					}\n \
				}\n",
			expected_result_error_test
		),
		/* Test18 */
		// TestCase(
		// 	"{\n
		// 		}\n",
		// 	expected_result_error_test
		// ),
		/* Test19 */
		TestCase(
			"server	{{{\n \
				}\n",
			expected_result_error_test
		),
		/* Test20 */
		TestCase(
			"server 	{\n \
				server_name server; ;;;;\n \
				}\n",
			expected_result_error_test
		),
		/* Test21 */
		TestCase(
			"server {\n \
				unknown test;\n \
				}\n",
			expected_result_error_test
		)
	};

	ret_code |= RunErrorTests(error_test_cases, ARRAY_SIZE(error_test_cases));

	return ret_code;
}
