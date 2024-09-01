#include "http_response.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>

namespace {

std::string LoadFileContent(const std::string &file_path) {
	std::ifstream file(file_path.c_str());
	if (!file) {
		std::cerr << "Error opening file: " << file_path << std::endl;
		return "";
	}
	std::ostringstream file_content;
	file_content << file.rdbuf();
	return file_content.str();
}

int GetTestCaseNum() {
	static unsigned int test_case_num = 0;
	++test_case_num;
	return test_case_num;
}

template <typename T>
int HandleResult(const T &result, const T &expected) {
	if (result == expected) {
		std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK]" << utils::color::RESET
				  << std::endl;
		return EXIT_SUCCESS;
	} else {
		std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
				  << std::endl;
		return EXIT_FAILURE;
	}
}

} // namespace

int main(void) {
	int ret_code = EXIT_SUCCESS;

	// http_method/expected
	// LF:   exist target resourse file
	std::string expected_file = LoadFileContent("expected/file.txt");
	// CRLF: use default status code file
	std::string expected_created = LoadFileContent("expected/created.txt");
	std::string expected_no_content = LoadFileContent("expected/no_content.txt");
	std::string expected_redirect = LoadFileContent("expected/redirect.txt");
	std::string expected_forbidden = LoadFileContent("expected/forbidden.txt");
	std::string expected_not_found = LoadFileContent("expected/not_found.txt");

	// ファイルが存在する場合
	std::string get_test1_response_body_message;
	http::HttpResponse::GetHandler("test/file.txt", get_test1_response_body_message);
	ret_code |= HandleResult(get_test1_response_body_message, expected_file);

	// ファイルが存在しない場合
	std::string  get_test2_response_body_message;
	http::HttpResponse::GetHandler("test/a",  get_test2_response_body_message);
	ret_code |= HandleResult( get_test2_response_body_message, expected_not_found);

	// ディレクトリの場合かつ'/'がない場合
	std::string get_test3_response_body_message;
	http::HttpResponse::GetHandler("test/directory", get_test3_response_body_message);
	ret_code |= HandleResult(get_test3_response_body_message, expected_redirect);

	// ファイルが権限ない場合
	std::string get_test4_response_body_message;
	http::HttpResponse::GetHandler("test/no_authority_file", get_test4_response_body_message);
	ret_code |= HandleResult(get_test4_response_body_message, expected_forbidden);

	// POST test
	// 新しいファイルをアップロードする場合
	std::string post_test1_request_body_message = "OK";
	std::string post_test1_response_body_message;
	http::HttpResponse::PostHandler(
		"ok.txt", post_test1_request_body_message, post_test1_response_body_message
	);
	ret_code |= HandleResult(post_test1_response_body_message, expected_created);

	// すでにアップロードされたファイルをアップロードする場合
	std::string post_test2_request_body_message = "OK";
	std::string post_test2_response_body_message;
	http::HttpResponse::PostHandler(
		"ok.txt", post_test2_request_body_message, post_test2_response_body_message
	);
	ret_code |= HandleResult(post_test2_response_body_message, expected_no_content);

	// ディレクトリの場合
	std::string post_test3_request_body_message;
	std::string post_test3_response_body_message;
	http::HttpResponse::PostHandler(
		"test/directory", post_test3_request_body_message, post_test3_response_body_message
	);
	ret_code |= HandleResult(post_test3_response_body_message, expected_forbidden);

	// DELETE test
	// ファイルが存在するかつ親ディレクトリが書き込み権限あるとき
	std::string delete_test1_response_body_message;
	http::HttpResponse::DeleteHandler("ok.txt", delete_test1_response_body_message);
	ret_code |= HandleResult(delete_test1_response_body_message, expected_no_content);

	// ファイルが存在しない場合
	std::string delete_test2_response_body_message;
	http::HttpResponse::DeleteHandler("not_found.txt", delete_test2_response_body_message);
	ret_code |= HandleResult(delete_test2_response_body_message, expected_not_found);

	// ディレクトリ内にファイルが存在してる場合
	std::string delete_test3_response_body_message;
	http::HttpResponse::DeleteHandler("test", delete_test3_response_body_message);
	ret_code |= HandleResult(delete_test3_response_body_message, expected_forbidden);

	// 　ディレクトリ内にファイルが存在してない場合
	std::string delete_test4_response_body_message;
	http::HttpResponse::DeleteHandler("test/s", delete_test4_response_body_message);
	ret_code |= HandleResult(delete_test4_response_body_message, expected_forbidden);

	// 存在しないディレクトリの場合
	std::string delete_test5_response_body_message;
	http::HttpResponse::DeleteHandler("not_found_directory", delete_test5_response_body_message);
	ret_code |= HandleResult(delete_test5_response_body_message, expected_not_found);

	// 書き込み権限がないディレクトリの中にあるファイル場合
	std::string delete_test6_response_body_message;
	http::HttpResponse::DeleteHandler(
		"test/no_authority_directory/test.txt", delete_test6_response_body_message
	);
	ret_code |= HandleResult(delete_test6_response_body_message, expected_forbidden);
	return ret_code;
}