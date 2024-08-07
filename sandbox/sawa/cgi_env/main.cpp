#include "cgi.hpp"
#include "cgi_parse.hpp"
#include "color.hpp"
#include <iostream>
#include <map>

// CGIを実行する関数
void run_cgi(const char *scirpt_name);

// メタ変数を作成する関数
// RFC3875 https://datatracker.ietf.org/doc/html/rfc3875#section-4.1
// meta-variable-name = "AUTH_TYPE" | "CONTENT_LENGTH" | "CONTENT_TYPE" | "GATEWAY_INTERFACE" |
// 						 "PATH_INFO" | "PATH_TRANSLATED" | "QUERY_STRING" | "REMOTE_ADDR" |
// 						 "REMOTE_HOST" | "REMOTE_IDENT" | "REMOTE_USER" | "REQUEST_METHOD" |
// 						 "SCRIPT_NAME" | "SERVER_NAME" | "SERVER_PORT" | "SERVER_PROTOCOL" |
// 						 "SERVER_SOFTWARE"
std::map<std::string, std::string> CreateRequestMetaVariables();

// メタ変数から環境変数を作成する関数
char **create_cgi_env(const std::map<std::string, std::string> &request_meta_variables);

void free_cgi_env(char *const *cgi_env) {
	for (size_t i = 0; cgi_env[i] != NULL; ++i) {
		delete[] cgi_env[i];
	}
	delete[] cgi_env;
}

// cgiスクリプトの実行権限を確認する関数
bool is_executable(const char *file_path);

void test_run_cgi(const char *script_name) {
	if (is_executable(script_name)) {
		std::cout << utils::color::GREEN << "[OK] " << utils::color::RESET << std::endl;
		std::cout << "The file " << script_name << " is executable." << std::endl;
		run_cgi(script_name);
	} else {
		std::cerr << utils::color::RED << "[NG] " << utils::color::RESET << std::endl;
		std::cerr << "The file " << script_name << " is not executable or does not exist."
				  << std::endl;
	}
}

int main(void) {
	const std::string expected[] = {
		"AUTH_TYPE",
		"CONTENT_LENGTH",
		"CONTENT_TYPE",
		"GATEWAY_INTERFACE",
		"PATH_INFO",
		"PATH_TRANSLATED",
		"QUERY_STRING",
		"REMOTE_ADDR",
		"REMOTE_HOST",
		"REMOTE_IDENT",
		"REMOTE_USER",
		"REQUEST_METHOD",
		"SCRIPT_NAME",
		"SERVER_NAME",
		"SERVER_PORT",
		"SERVER_PROTOCOL",
		"SERVER_SOFTWARE"
	};
	std::map<std::string, std::string> request_meta_variables = CreateRequestMetaVariables();
	const size_t                       expected_size = sizeof(expected) / sizeof(expected[0]);

	// メタ変数の値があるかどうか確認するテスト（expectedはRFC3875のメタ変数の値をコピペした）
	for (size_t i = 0; i < expected_size; ++i) {
		if (request_meta_variables.find(expected[i]) != request_meta_variables.end()) {
			std::cout << utils::color::GREEN << "[OK] " << utils::color::RESET << expected[i]
					  << std::endl;
		} else {
			std::cerr << utils::color::RED << "[NG] " << utils::color::RESET
					  << "Missing: " << expected[i] << std::endl;
		}
	}

	// 環境変数を出力する（=が付与されてるか、valueが紐づいてるかどうか、動的にメモリが確保できてるか）
	char *const *cgi_env = create_cgi_env(request_meta_variables);
	for (size_t i = 0; cgi_env[i] != NULL; i++) {
		std::cout << cgi_env[i] << std::endl;
	}
	free_cgi_env(cgi_env);

	// run_cgi テスト
	// - 親プロセスをkillせずにCGIスクリプトが実行できているかどうか
	// - CGIスクリプトが標準出力されてるか？（first.plが標準出力してればOK）
	// - script_nameは実行権限があるかどうか、存在しているかどうか

	// test_run_cgi("../../../test/apache/cgi/print_stdout.pl");
	// test_run_cgi("../../../test/apache/cgi/print_stdin.pl");
	// test_run_cgi("../../../test/apache/cgi/print_env.pl");
	// test_run_cgi("../../../test/apache/cgi/not_executable.pl");
	// test_run_cgi("../../../test/apache/cgi/not_file.pl");

	// cgi::CgiRequest request;
	// request.meta_variables = CreateRequestMetaVariables();
	// request.body_message   = "name=ChatGPT&message=Hello";
	cgi::Cgi    cgi;
	std::string http_request = "tmp";
	// std::string http_request = "POST /cgi-bin/print_stdin.pl HTTP/1.1\r\n"
	// 							"Host: localhost:8080\r\n"
	// 							"Server: Apache/2.4.59 (Unix)\r\n"
	// 							"Content-Type: application/x-www-form-urlencoded\r\n"
	// 							"Content-Length: 26\r\n"
	// 							"\r\n"
	// 							"name=ChatGPT&message=Hello";
	cgi::CgiRequest request = cgi::CgiParse::Parse(http_request);
	cgi.Run(request);
	return 0;
}
