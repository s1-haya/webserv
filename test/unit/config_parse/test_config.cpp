#include "color.hpp"
#include "config.hpp"
#include "context.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
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
	   << "autoindex: " << std::boolalpha << location.autoindex << ", "
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

Result Run(const std::string &file_path, const ServerList &expected) {
	Result run_result;

	ConfigInstance->Create(file_path);
	if (ConfigInstance->servers_ != expected) {
		std::ostringstream error_log;
		error_log << "- Expected: [ " << expected << " ]\n";
		error_log << "- Result  : [ " << ConfigInstance->servers_ << " ]\n";
		run_result.is_success = false;
		run_result.error_log  = error_log.str();
	}
	ConfigInstance->Destroy();
	return run_result;
}

std::string ReadFile(const std::string &file_path) {
	std::ifstream file(file_path.c_str());
	if (!file) {
		throw std::runtime_error("Cannot open file"); // どこかでキャッチすべき？
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int Test(const Result &result, const std::string &file_path) {
	if (result.is_success) {
		PrintOk();
		return EXIT_SUCCESS;
	} else {
		PrintNg();
		PrintError("ConfigParser failed:");
		std::cerr << "src:[\n" << ReadFile(file_path) << "]" << std::endl;
		std::cerr << "--------------------------------" << std::endl;
		std::cerr << result.error_log;
		return EXIT_FAILURE;
	}
}

/* For Error Tests */
int RunErrorTest(const std::string &file_path, const ServerList &expected, const std::string &src) {
	int ret_code = EXIT_SUCCESS;

	try {
		Run(file_path, expected);
		PrintNg();
		PrintError("ConfigParser failed (No Throw):");
		std::cerr << "src:[\n" << src << "]" << std::endl;
		ret_code |= EXIT_FAILURE;
	} catch (const std::exception &e) {
		PrintOk();
		utils::Debug(e.what());
	}
	return ret_code;
}

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

/* Test8 Additional Directives */
ServerList MakeExpectedTest8() {
	ServerList     expected_result;
	std::list<int> expected_ports_1;
	expected_ports_1.push_back(8080);
	std::list<std::string> server_names_1;
	server_names_1.push_back("localhost");
	LocationList           expected_locationlist_1;
	std::list<std::string> allowed_methods_1;
	allowed_methods_1.push_back("GET");
	allowed_methods_1.push_back("POST");
	std::pair<unsigned int, std::string> redirect_1(302, "/redirect.html");
	context::LocationCon                 expected_location_1_1 =
		BuildLocationCon("/", "/data/", "index.html", true, allowed_methods_1, redirect_1);
	expected_locationlist_1.push_back(expected_location_1_1);
	std::pair<unsigned int, std::string> error_page_1(404, "/404.html");
	context::ServerCon                   expected_server_1 = BuildServerCon(
        expected_ports_1, server_names_1, expected_locationlist_1, 2024, error_page_1
    );
	expected_result.push_back(expected_server_1);

	return expected_result;
}

} // namespace

int main() {
	int ret_code = EXIT_SUCCESS;

	std::cout << "Normal Tests" << std::endl;
	ret_code |= Test(Run("test_file/test1.conf", MakeExpectedTest1()), "test_file/test1.conf");
	ret_code |= Test(Run("test_file/test2.conf", MakeExpectedTest2()), "test_file/test2.conf");
	ret_code |= Test(Run("test_file/test3.conf", MakeExpectedTest3()), "test_file/test3.conf");
	ret_code |= Test(Run("test_file/test4.conf", MakeExpectedTest4()), "test_file/test4.conf");
	ret_code |= Test(Run("test_file/test5.conf", MakeExpectedTest5()), "test_file/test5.conf");
	ret_code |= Test(Run("test_file/test6.conf", MakeExpectedTest6()), "test_file/test6.conf");
	ret_code |= Test(Run("test_file/test7.conf", MakeExpectedTest7()), "test_file/test7.conf");
	ret_code |= Test(Run("test_file/test8.conf", MakeExpectedTest8()), "test_file/test8.conf");

	std::cout << std::endl;
	std::cout << "Error Tests" << std::endl;
	ServerList expected;
	ret_code |= RunErrorTest(
		"test_file_error/error_test1.conf", expected, "test_file_error/error_test1.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test2.conf", expected, "test_file_error/error_test2.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test3.conf", expected, "test_file_error/error_test3.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test4.conf", expected, "test_file_error/error_test4.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test5.conf", expected, "test_file_error/error_test5.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test6.conf", expected, "test_file_error/error_test6.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test7.conf", expected, "test_file_error/error_test7.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test8.conf", expected, "test_file_error/error_test8.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test9.conf", expected, "test_file_error/error_test9.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test10.conf", expected, "test_file_error/error_test10.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test11.conf", expected, "test_file_error/error_test11.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test12.conf", expected, "test_file_error/error_test12.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test13.conf", expected, "test_file_error/error_test13.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test14.conf", expected, "test_file_error/error_test14.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test15.conf", expected, "test_file_error/error_test15.conf"
	);
	ret_code |= RunErrorTest(
		"test_file_error/error_test16.conf", expected, "test_file_error/error_test16.conf"
	);

	return ret_code;
}
