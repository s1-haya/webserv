#include "tmp_http.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

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
	int ret_code = 0;

	// リクエストラインの書式が正しい場合
	http::TmpHttp test1;
	test1.ParseHttpRequestFormat(1, "GET / HTTP/1.1\r\n");
	ret_code |= HandleResult(test1.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test1.GetIsRequestLineFormat(1), true);

	// リクエストラインの書式が正しいくない場合
	http::TmpHttp test2;
	test2.ParseHttpRequestFormat(1, "GET / \r\n");
	ret_code |= HandleResult(test2.GetStatusCode(1), http::BAD_REQUEST);
	ret_code |= HandleResult(test2.GetIsRequestLineFormat(1), false);

	// CRLNがない場合
	http::TmpHttp test3;
	test3.ParseHttpRequestFormat(1, "GET / ");
	// 本来のステータスコードはRequest Timeout
	ret_code |= HandleResult(test3.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test3.GetIsRequestLineFormat(1), false);

	// ヘッダフィールドの書式が正しい場合
	http::TmpHttp test1_header_fileds;
	test1_header_fileds.ParseHttpRequestFormat(1, "GET / HTTP/1.1\r\nHost: a\r\n\r\n");
	ret_code |= HandleResult(test1_header_fileds.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test1_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test1_header_fileds.GetIsHeaderFieldsFormat(1), true);
	ret_code |= HandleResult(test1_header_fileds.GetIsBodyMessageFormat(1), true);

	// ヘッダフィールドの書式が正しくない場合
	http::TmpHttp test2_header_fileds;
	test2_header_fileds.ParseHttpRequestFormat(1, "GET / HTTP/1.1\r\nHost :\r\n\r\n");
	ret_code |= HandleResult(test2_header_fileds.GetStatusCode(1), http::BAD_REQUEST);
	ret_code |= HandleResult(test2_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test2_header_fileds.GetIsHeaderFieldsFormat(1), false);
	ret_code |= HandleResult(test2_header_fileds.GetIsBodyMessageFormat(1), false);

	// ヘッダフィールドにContent-Lengthがある場合
	http::TmpHttp test3_header_fileds;
	test3_header_fileds.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\nab"
	);
	ret_code |= HandleResult(test3_header_fileds.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test3_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test3_header_fileds.GetIsHeaderFieldsFormat(1), true);
	ret_code |= HandleResult(test3_header_fileds.GetIsBodyMessageFormat(1), true);

	// Content-Lengthの数値以上にボディメッセージがある場合
	http::TmpHttp test4_header_fileds;
	test4_header_fileds.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\nabccccccccc"
	);
	const std::string &test4_body_message = "ab";
	ret_code |= HandleResult(test4_header_fileds.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test4_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test4_header_fileds.GetIsHeaderFieldsFormat(1), true);
	ret_code |= HandleResult(test4_header_fileds.GetIsBodyMessageFormat(1), true);
	ret_code |= HandleResult(test4_header_fileds.GetBodyMessage(1), test4_body_message);

	http::TmpHttp test5_header_fileds;
	test5_header_fileds.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length: dddd\r\n\r\nabccccccccc"
	);
	ret_code |= HandleResult(test5_header_fileds.GetStatusCode(1), http::BAD_REQUEST);
	ret_code |= HandleResult(test5_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test5_header_fileds.GetIsHeaderFieldsFormat(1), true);
	ret_code |= HandleResult(test5_header_fileds.GetIsBodyMessageFormat(1), false);

	// ボディメッセージを追加で送信した場合
	http::TmpHttp test6_header_fileds;
	test6_header_fileds.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length:  3\r\n\r\na"
	);
	ret_code |= HandleResult(test6_header_fileds.GetStatusCode(1), http::OK);
	ret_code |= HandleResult(test6_header_fileds.GetIsRequestLineFormat(1), true);
	ret_code |= HandleResult(test6_header_fileds.GetIsHeaderFieldsFormat(1), true);
	ret_code |= HandleResult(test6_header_fileds.GetIsBodyMessageFormat(1), false);
	test6_header_fileds.ParseHttpRequestFormat(1, "bc");
	const std::string &test6_body_message = "abc";
	ret_code |= HandleResult(test6_header_fileds.GetIsBodyMessageFormat(1), true);
	ret_code |= HandleResult(test6_header_fileds.GetBodyMessage(1), test6_body_message);

	// リクエストのステータスコードが200の場合
	http::TmpHttp test1_response;
	test1_response.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length:  3\r\n\r\na"
	);
	const std::string test1_expected_response =
		"HTTP/1.1 200 OK\r\nConnection: close\r\nHost: sawa\r\n\r\nYou can't connect the dots "
		"looking forword. You can only connect the dots looking backwards";
	ret_code |= HandleResult(test1_response.CreateHttpResponse(1), test1_expected_response);

	// リクエストのステータスコードが400の場合
	http::TmpHttp test2_response;
	test2_response.ParseHttpRequestFormat(
		1, "GET / HTTP/1.\r\nHost test\r\nContent-Length:  3\r\n\r\na"
	);
	std::string test2_expected_response = LoadFileContent("expected_response/test2.txt");
	ret_code |= HandleResult(test2_response.CreateHttpResponse(1), test2_expected_response);
	return ret_code;
}
