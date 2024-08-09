#include "tmp_http.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

struct TestCase {
	TestCase(
		int client_fd, const std::string &read_buf, const http::HttpRequestParsedData &expected
	)
		: client_fd(client_fd), read_buf(read_buf), expected(expected) {}
	int                               client_fd;
	const std::string                 read_buf;
	const http::HttpRequestParsedData expected;
};

struct Result {
	Result() : is_success(true) {}
	bool        is_success;
	std::string error_log;
};

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
bool IsSame(const T &result, const T &expected) {
	return result == expected;
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

int HandleResult(const Result &result) {
	if (result.is_success) {
		std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK]" << utils::color::RESET
				  << std::endl;
		return EXIT_SUCCESS;
	} else {
		std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
				  << std::endl;
		std::cerr << result.error_log;
		return EXIT_FAILURE;
	}
}

bool IsSameHttpRequestFormat(
	const http::IsHttpRequestFormat &result, const http::IsHttpRequestFormat &expected
) {
	return IsSame(result.is_request_line, expected.is_request_line) &&
		   IsSame(result.is_header_fields, expected.is_header_fields) &&
		   IsSame(result.is_body_message, expected.is_body_message);
}

Result IsSameHttpRequestParsedData(
	const http::HttpRequestParsedData &result, const http::HttpRequestParsedData &expected
) {
	Result             request_parsed_result;
	std::ostringstream oss;
	if (!IsSame(result.request_result.status_code, expected.request_result.status_code)) {
		oss << "Error: Status Code\n";
		oss << "- Expected: [" << expected.request_result.status_code << "]\n";
		oss << "- Result  : [" << result.request_result.status_code << "]\n";
		request_parsed_result.is_success = false;
	}
	if (!IsSameHttpRequestFormat(result.is_request_format, expected.is_request_format)) {
		oss << "Error: Is Http Request Format\n";
		oss << "Request Line\n";
		oss << "- Expected: [" << std::boolalpha << expected.is_request_format.is_request_line
			<< "]\n";
		oss << "- Result  : [" << std::boolalpha << result.is_request_format.is_request_line
			<< "]\n";
		oss << "Header Fields\n";
		oss << "- Expected: [" << std::boolalpha << expected.is_request_format.is_header_fields
			<< "]\n";
		oss << "- Result  : [" << std::boolalpha << result.is_request_format.is_header_fields
			<< "]\n";
		oss << "Body Message\n";
		oss << "- Expected: [" << std::boolalpha << expected.is_request_format.is_body_message
			<< "]\n";
		oss << "- Result  : [" << std::boolalpha << result.is_request_format.is_body_message
			<< "]\n";
		request_parsed_result.is_success = false;
	}
	request_parsed_result.error_log = oss.str();
	return request_parsed_result;
}

int Run(int client_fd, const std::string &read_buf, const http::HttpRequestParsedData &expected) {
	http::TmpHttp http;
	http.ParseHttpRequestFormat(client_fd, read_buf);
	const Result &result = IsSameHttpRequestParsedData(http.GetClientData(client_fd), expected);
	return HandleResult(result);
}

int RunTestCases(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = 0;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		ret_code |= Run(test_case.client_fd, test_case.read_buf, test_case.expected);
	}
	return ret_code;
}

} // namespace

int main(void) {
	int ret_code = 0;

	// todo: http/http_response/test_http_request.cpp HttpRequestParsedData関数のテストケース

	// 1.リクエストラインの書式が正しい場合
	http::HttpRequestParsedData test1_request_line;
	test1_request_line.request_result.status_code        = http::OK;
	test1_request_line.is_request_format.is_request_line = true;

	// 2.リクエストラインの書式が正しいくない場合
	http::HttpRequestParsedData test2_request_line;
	test2_request_line.request_result.status_code        = http::BAD_REQUEST;
	test2_request_line.is_request_format.is_request_line = false;

	// 3.CRLNがない場合
	http::HttpRequestParsedData test3_request_line;
	// 本来のステータスコードはRequest Timeout
	test3_request_line.request_result.status_code        = http::OK;
	test3_request_line.is_request_format.is_request_line = false;

	static const TestCase test_case_http_request_line_format[] = {
		TestCase(1, "GET / HTTP/1.1\r\n", test1_request_line),
		TestCase(1, "GET / HTTP/1.\r\n", test2_request_line),
		TestCase(1, "GET / HTTP/1.1", test3_request_line)
	};

	ret_code |= RunTestCases(
		test_case_http_request_line_format,
		sizeof(test_case_http_request_line_format) / sizeof(test_case_http_request_line_format[0])
	);

	// 4.ヘッダフィールドの書式が正しい場合
	http::HttpRequestParsedData test1_header_fileds;
	test1_header_fileds.request_result.status_code         = http::OK;
	test1_header_fileds.is_request_format.is_request_line  = true;
	test1_header_fileds.is_request_format.is_header_fields = true;
	test1_header_fileds.is_request_format.is_body_message  = true;

	// 5.ヘッダフィールドの書式が正しくない場合
	http::HttpRequestParsedData test2_header_fileds;
	test2_header_fileds.request_result.status_code         = http::BAD_REQUEST;
	test2_header_fileds.is_request_format.is_request_line  = true;
	test2_header_fileds.is_request_format.is_header_fields = false;
	test2_header_fileds.is_request_format.is_body_message  = false;

	// 6.ヘッダフィールドにContent-Lengthがあるがボディメッセージがない場合
	http::HttpRequestParsedData test3_header_fileds;
	// 本来のステータスコードはRequest Timeout
	test3_header_fileds.request_result.status_code         = http::OK;
	test3_header_fileds.is_request_format.is_request_line  = true;
	test3_header_fileds.is_request_format.is_header_fields = true;
	test3_header_fileds.is_request_format.is_body_message  = false;

	static const TestCase test_case_http_request_header_fields_format[] = {
		TestCase(1, "GET / HTTP/1.1\r\nHost: a\r\n\r\n", test1_header_fileds),
		TestCase(1, "GET / HTTP/1.1\r\nHost :\r\n\r\n", test2_header_fileds),
		TestCase(
			1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\n", test3_header_fileds
		),
	};

	ret_code |= RunTestCases(
		test_case_http_request_header_fields_format,
		sizeof(test_case_http_request_header_fields_format) /
			sizeof(test_case_http_request_header_fields_format[0])
	);

	// 7.ボディメッセージが正しい場合
	http::HttpRequestParsedData test1_body_message;
	test1_body_message.request_result.status_code         = http::OK;
	test1_body_message.is_request_format.is_request_line  = true;
	test1_body_message.is_request_format.is_header_fields = true;
	test1_body_message.is_request_format.is_body_message  = true;

	// 8.Content-Lengthの数値以上にボディメッセージがある場合
	http::HttpRequestParsedData test2_body_message;
	test2_body_message.request_result.status_code          = http::OK;
	test2_body_message.is_request_format.is_request_line   = true;
	test2_body_message.is_request_format.is_header_fields  = true;
	test2_body_message.is_request_format.is_body_message   = true;
	test2_body_message.request_result.request.body_message = "ab";

	// 9.Content-Lengthの値の書式が間違ってる場合
	http::HttpRequestParsedData test3_body_message;
	test3_body_message.request_result.status_code          = http::BAD_REQUEST;
	test3_body_message.is_request_format.is_request_line   = true;
	test3_body_message.is_request_format.is_header_fields  = true;
	test3_body_message.is_request_format.is_body_message   = false;
	test3_body_message.request_result.request.body_message = "ab";

	static const TestCase test_case_http_request_body_message_format[] = {
		TestCase(
			1, "GET / HTTP/1.1\r\nHost: a\r\n\r\nContent-Length:  3\r\n\r\nabc", test1_body_message
		),
		TestCase(
			1,
			"GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\nabccccccccc",
			test2_body_message
		),
		TestCase(
			1,
			"GET / HTTP/1.1\r\nHost: test\r\nContent-Length: dddd\r\n\r\nabccccccccc",
			test3_body_message
		),
	};

	ret_code |= RunTestCases(
		test_case_http_request_body_message_format,
		sizeof(test_case_http_request_body_message_format) /
			sizeof(test_case_http_request_body_message_format[0])
	);

	// 10.ボディメッセージを追加で送信した場合
	http::TmpHttp               test4_body_message;
	http::HttpRequestParsedData test4_expected_body_message;
	test4_expected_body_message.request_result.status_code          = http::OK;
	test4_expected_body_message.is_request_format.is_request_line   = true;
	test4_expected_body_message.is_request_format.is_header_fields  = true;
	test4_expected_body_message.is_request_format.is_body_message   = false;
	test4_expected_body_message.request_result.request.body_message = "abc";

	test4_body_message.ParseHttpRequestFormat(
		1, "GET / HTTP/1.1\r\nHost: test\r\nContent-Length:  3\r\n\r\na"
	);
	Result result = IsSameHttpRequestParsedData(
		test4_body_message.GetClientData(1), test4_expected_body_message
	);
	ret_code |= HandleResult(result);

	// 11.追加でボディメッセージを送信
	test4_body_message.ParseHttpRequestFormat(1, "bc");
	test4_expected_body_message.is_request_format.is_body_message = true;
	result                                                        = IsSameHttpRequestParsedData(
        test4_body_message.GetClientData(1), test4_expected_body_message
    );
	ret_code |= HandleResult(result);

	// todo: http/http_response/test_http_response.cpp responseを確認、実行、作成のテスト
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
