#include "http_parse.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {

struct TestCase {
	TestCase(const std::string &read_buf, const http::HttpRequestParsedData &expected)
		: read_buf(read_buf), expected(expected) {}
	const std::string                 read_buf;
	const http::HttpRequestParsedData expected;
};

struct Result {
	Result() : is_success(true) {}
	bool        is_success;
	std::string error_log;
};

http::RequestLine CreateRequestLine(
	const std::string &method, const std::string &request_target, const std::string &version
) {
	http::RequestLine request_line;
	request_line.method         = method;
	request_line.request_target = request_target;
	request_line.version        = version;
	return request_line;
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

Result IsSameHttpRequestFormat(
	const http::IsHttpRequestFormat &result, const http::IsHttpRequestFormat &expected
) {
	Result request_format_result;
	if (!(IsSame(result.is_request_line, expected.is_request_line) &&
		  IsSame(result.is_header_fields, expected.is_header_fields) &&
		  IsSame(result.is_body_message, expected.is_body_message))) {
		std::ostringstream error_log;
		error_log << "Error: Is Http Request Format\n";
		error_log << "Request Line\n";
		error_log << "- Expected: [" << std::boolalpha << expected.is_request_line << "]\n";
		error_log << "- Result  : [" << std::boolalpha << result.is_request_line << "]\n";
		error_log << "Header Fields\n";
		error_log << "- Expected: [" << std::boolalpha << expected.is_header_fields << "]\n";
		error_log << "- Result  : [" << std::boolalpha << result.is_header_fields << "]\n";
		error_log << "Body Message\n";
		error_log << "- Expected: [" << std::boolalpha << expected.is_body_message << "]\n";
		error_log << "- Result  : [" << std::boolalpha << result.is_body_message << "]\n";
		request_format_result.is_success = false;
		request_format_result.error_log  = error_log.str();
	}
	return request_format_result;
}

Result IsSameStatusCode(const http::StatusCode &status_code, const http::StatusCode &expected) {
	Result status_code_result;
	if (status_code.GetEStatusCode() != expected.GetEStatusCode()) {
		std::ostringstream error_log;
		error_log << "Error: Status Code\n";
		error_log << "- Expected: [" << expected.GetStatusCode() << "]\n";
		error_log << "- Result  : [" << status_code.GetStatusCode() << "]\n";
		status_code_result.is_success = false;
		status_code_result.error_log  = error_log.str();
	}
	return status_code_result;
}

Result IsSameRequestLine(const http::RequestLine &res, const http::RequestLine &expected) {
	Result             request_line_result;
	std::ostringstream error_log;
	if (expected.method != res.method) {
		error_log << "Error: Method\n";
		error_log << "- Expected: [" << expected.method << "]\n";
		error_log << "- Result  : [" << res.method << "]\n";
		request_line_result.is_success = false;
	}
	if (expected.request_target != res.request_target) {
		error_log << "Error: Request Target\n";
		error_log << "- Expected: [" << expected.request_target << "]\n";
		error_log << "- Result  : [" << res.request_target << "]\n";
		request_line_result.is_success = false;
	}
	if (expected.version != res.version) {
		error_log << "Error: Version\n";
		error_log << "- Expected: [" << expected.version << "]\n";
		error_log << "- Result  : [" << res.version << "]\n";
		request_line_result.is_success = false;
	}
	request_line_result.error_log = error_log.str();
	return request_line_result;
}

Result IsSameHeaderFields(const http::HeaderFields &res, http::HeaderFields expected) {
	Result             header_fields_result;
	std::ostringstream error_log;
	if (expected["Host"].size() && expected.at("Host") != res.at("Host")) {
		error_log << "Error: Host\n";
		error_log << "- Expected: [" << expected.at("Host") << "]\n";
		error_log << "- Result  : [" << res.at("Host") << "]\n";
		header_fields_result.is_success = false;
	}
	if (expected["Connection"].size() && expected.at("Connection") != res.at("Connection")) {
		error_log << "Error: Connection\n";
		error_log << "- Expected: [" << expected.at("Connection") << "]\n";
		error_log << "- Result  : [" << res.at("Connection") << "]\n";
		header_fields_result.is_success = false;
	}
	if (expected["Content-Length"].size() &&
		expected.at("Content-Length") != res.at("Content-Length")) {
		error_log << "Error: Content-Length\n";
		error_log << "- Expected: [" << expected.at("Content-Length") << "]\n";
		error_log << "- Result  : [" << res.at("Content-Length") << "]\n";
		header_fields_result.is_success = false;
	}
	if (expected["Transfer-Encoding"].size() &&
		expected.at("Transfer-Encoding") != res.at("Transfer-Encoding")) {
		error_log << "Error: Transfer-Encoding\n";
		error_log << "- Expected: [" << expected.at("Transfer-Encoding") << "]\n";
		error_log << "- Result  : [" << res.at("Transfer-Encoding") << "]\n";
		header_fields_result.is_success = false;
	}
	header_fields_result.error_log = error_log.str();
	return header_fields_result;
}

Result IsSameBodyMessage(const std::string &res, const std::string expected) {
	Result             body_message_result;
	std::ostringstream error_log;
	if (res != expected) {
		error_log << "Error: body_message\n";
		error_log << "- Expected: [" << expected << "]\n";
		error_log << "- Result  : [" << res << "]\n";
		body_message_result.is_success = false;
	}
	body_message_result.error_log = error_log.str();
	return body_message_result;
}

Result
IsSameHttpRequest(const http::HttpRequestFormat &res, const http::HttpRequestFormat &expected) {
	Result request_line_result = IsSameRequestLine(res.request_line, expected.request_line);
	if (!(request_line_result.is_success)) {
		return request_line_result;
	}
	Result header_fields_result = IsSameHeaderFields(res.header_fields, expected.header_fields);
	if (!(header_fields_result.is_success)) {
		return header_fields_result;
	}
	Result body_message_result = IsSameBodyMessage(res.body_message, expected.body_message);
	if (!(body_message_result.is_success)) {
		return body_message_result;
	}
	Result http_request_result;
	return http_request_result;
}

Result IsSameHttpRequestParsedData(
	const http::HttpRequestParsedData &result, const http::HttpRequestParsedData &expected
) {
	Result status_code_result =
		IsSameStatusCode(result.request_result.status_code, expected.request_result.status_code);
	if (!status_code_result.is_success) {
		return status_code_result;
	}
	Result is_http_request_format_result =
		IsSameHttpRequestFormat(result.is_request_format, expected.is_request_format);
	if (!is_http_request_format_result.is_success) {
		return is_http_request_format_result;
	}
	Result http_request_result;
	bool   is_response_complete = result.is_request_format.is_request_line &&
								result.is_request_format.is_header_fields &&
								result.is_request_format.is_body_message;
	if (is_response_complete) {
		http_request_result =
			IsSameHttpRequest(result.request_result.request, expected.request_result.request);
	}
	return http_request_result;
}

http::HttpRequestParsedData ParseHttpRequestFormat(const std::string &read_buf) {
	http::HttpRequestParsedData save_data;
	save_data.current_buf += read_buf;
	try {
		http::HttpParse::TmpRunHttpResultVersion(save_data);
	} catch (const http::HttpException &e) {
		save_data.request_result.status_code = e.GetStatusCode();
	}
	return save_data;
}

int Run(const std::string &read_buf, const http::HttpRequestParsedData &expected) {
	const Result &result = IsSameHttpRequestParsedData(ParseHttpRequestFormat(read_buf), expected);
	return HandleResult(result);
}

int RunTestCases(const TestCase test_cases[], std::size_t num_test_cases) {
	int ret_code = 0;

	for (std::size_t i = 0; i < num_test_cases; i++) {
		const TestCase test_case = test_cases[i];
		ret_code |= Run(test_case.read_buf, test_case.expected);
	}
	return ret_code;
}

} // namespace

int main(void) {
	int ret_code = 0;

	// todo: http/http_response/test_http_request.cpp HttpRequestParsedData関数のテストケース

	// 1.リクエストラインの書式が正しい場合
	http::HttpRequestParsedData test1_request_line;
	test1_request_line.request_result.status_code        = http::StatusCode(http::OK);
	test1_request_line.is_request_format.is_request_line = true;

	// 2.リクエストラインの書式が正しいくない場合
	http::HttpRequestParsedData test2_request_line;
	test2_request_line.request_result.status_code        = http::StatusCode(http::BAD_REQUEST);
	test2_request_line.is_request_format.is_request_line = false;

	// 3.CRLNがない場合
	http::HttpRequestParsedData test3_request_line;
	// 本来のステータスコードはRequest Timeout
	test3_request_line.request_result.status_code        = http::StatusCode(http::OK);
	test3_request_line.is_request_format.is_request_line = false;

	static const TestCase test_case_http_request_line_format[] = {
		TestCase("GET / HTTP/1.1\r\n", test1_request_line),
		TestCase("GET / HTTP/1.\r\n", test2_request_line),
		TestCase("GET / HTTP/1.1", test3_request_line)
	};

	ret_code |= RunTestCases(
		test_case_http_request_line_format,
		sizeof(test_case_http_request_line_format) / sizeof(test_case_http_request_line_format[0])
	);

	// 4.ヘッダフィールドの書式が正しい場合
	http::HttpRequestParsedData test1_header_fields;
	test1_header_fields.request_result.status_code = http::StatusCode(http::OK);
	test1_header_fields.request_result.request.request_line =
		CreateRequestLine("GET", "/", "HTTP/1.1");
	test1_header_fields.is_request_format.is_request_line  = true;
	test1_header_fields.is_request_format.is_header_fields = true;
	test1_header_fields.is_request_format.is_body_message  = true;

	// 5.ヘッダフィールドの書式が正しくない場合
	http::HttpRequestParsedData test2_header_fields;
	test2_header_fields.request_result.status_code         = http::StatusCode(http::BAD_REQUEST);
	test2_header_fields.is_request_format.is_request_line  = true;
	test2_header_fields.is_request_format.is_header_fields = false;
	test2_header_fields.is_request_format.is_body_message  = false;

	// 6.ヘッダフィールドにContent-Lengthがあるがボディメッセージがない場合
	http::HttpRequestParsedData test3_header_fields;
	// 本来のステータスコードはRequest Timeout
	test3_header_fields.request_result.status_code = http::StatusCode(http::OK);
	test3_header_fields.request_result.request.request_line =
		CreateRequestLine("GET", "/", "HTTP/1.1");
	test3_header_fields.is_request_format.is_request_line  = true;
	test3_header_fields.is_request_format.is_header_fields = true;
	test3_header_fields.is_request_format.is_body_message  = false;

	static const TestCase test_case_http_request_header_fields_format[] = {
		TestCase("GET / HTTP/1.1\r\nHost: a\r\n\r\n", test1_header_fields),
		TestCase("GET / HTTP/1.1\r\nHost :\r\n\r\n", test2_header_fields),
		TestCase("GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\n", test3_header_fields),
	};

	ret_code |= RunTestCases(
		test_case_http_request_header_fields_format,
		sizeof(test_case_http_request_header_fields_format) /
			sizeof(test_case_http_request_header_fields_format[0])
	);

	// 7.ボディメッセージが正しい場合
	http::HttpRequestParsedData test1_body_message;
	test1_body_message.request_result.status_code = http::StatusCode(http::OK);
	test1_body_message.request_result.request.request_line =
		CreateRequestLine("GET", "/", "HTTP/1.1");
	test1_body_message.is_request_format.is_request_line  = true;
	test1_body_message.is_request_format.is_header_fields = true;
	test1_body_message.is_request_format.is_body_message  = true;

	// 8.Content-Lengthの数値以上にボディメッセージがある場合
	http::HttpRequestParsedData test2_body_message;
	test2_body_message.request_result.status_code = http::StatusCode(http::OK);
	test2_body_message.request_result.request.request_line =
		CreateRequestLine("GET", "/", "HTTP/1.1");
	test2_body_message.is_request_format.is_request_line   = true;
	test2_body_message.is_request_format.is_header_fields  = true;
	test2_body_message.is_request_format.is_body_message   = true;
	test2_body_message.request_result.request.body_message = "ab";

	// 9.Content-Lengthの値の書式が間違ってる場合
	http::HttpRequestParsedData test3_body_message;
	test3_body_message.request_result.status_code          = http::StatusCode(http::BAD_REQUEST);
	test3_body_message.is_request_format.is_request_line   = true;
	test3_body_message.is_request_format.is_header_fields  = true;
	test3_body_message.is_request_format.is_body_message   = false;
	test3_body_message.request_result.request.body_message = "ab";

	// 10.Chunked Transfer-Encodingの場合
	http::HttpRequestParsedData test4_body_message;
	test4_body_message.request_result.status_code = http::StatusCode(http::OK);
	test4_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test4_body_message.is_request_format.is_request_line  = true;
	test4_body_message.is_request_format.is_header_fields = true;
	test4_body_message.is_request_format.is_body_message  = true;
	test4_body_message.request_result.request.body_message =
		"Wikipedia is a free online encyclopedia that anyone can edit.";

	// 11.Chunked Transfer-Encodingの場合で、chunk-sizeとchunk-dataの大きさが一致していない場合
	http::HttpRequestParsedData test5_body_message;
	test5_body_message.request_result.status_code = http::StatusCode(http::BAD_REQUEST);
	test5_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test5_body_message.is_request_format.is_request_line   = true;
	test5_body_message.is_request_format.is_header_fields  = true;
	test5_body_message.is_request_format.is_body_message   = false;
	test5_body_message.request_result.request.body_message = "Wikipedia";

	// 12.Chunked Transfer-Encodingの場合で、Content-Lengthがある場合
	http::HttpRequestParsedData test6_body_message;
	test6_body_message.request_result.status_code = http::StatusCode(http::BAD_REQUEST);
	test6_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test6_body_message.is_request_format.is_request_line   = true;
	test6_body_message.is_request_format.is_header_fields  = true;
	test6_body_message.is_request_format.is_body_message   = false;
	test6_body_message.request_result.request.body_message = "Wikipedia";

	// 13.Chunked Transfer-Encodingの場合で、終端に0\r\n\r\nがない場合
	http::HttpRequestParsedData test7_body_message;
	test7_body_message.request_result.status_code = http::StatusCode(http::OK);
	test7_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test7_body_message.is_request_format.is_request_line   = true;
	test7_body_message.is_request_format.is_header_fields  = true;
	test7_body_message.is_request_format.is_body_message   = false;
	test7_body_message.request_result.request.body_message = "Wikipedia";

	// 14.Chunked Transfer-Encodingの場合で、chunk-sizeが不正な場合(無効な文字)
	http::HttpRequestParsedData test8_body_message;
	test8_body_message.request_result.status_code = http::StatusCode(http::BAD_REQUEST);
	test8_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test8_body_message.is_request_format.is_request_line   = true;
	test8_body_message.is_request_format.is_header_fields  = true;
	test8_body_message.is_request_format.is_body_message   = false;
	test8_body_message.request_result.request.body_message = "Wikipedia";

	// 15.Chunked Transfer-Encodingの場合で、chunk-sizeが不正な場合(負の数)
	http::HttpRequestParsedData test9_body_message;
	test9_body_message.request_result.status_code = http::StatusCode(http::BAD_REQUEST);
	test9_body_message.request_result.request.request_line =
		CreateRequestLine("POST", "/", "HTTP/1.1");
	test9_body_message.is_request_format.is_request_line   = true;
	test9_body_message.is_request_format.is_header_fields  = true;
	test9_body_message.is_request_format.is_body_message   = false;
	test9_body_message.request_result.request.body_message = "Wikipedia";

	static const TestCase test_case_http_request_body_message_format[] = {
		TestCase(
			"GET / HTTP/1.1\r\nHost: a\r\n\r\nContent-Length:  3\r\n\r\nabc", test1_body_message
		),
		TestCase(
			"GET / HTTP/1.1\r\nHost: test\r\nContent-Length: 2\r\n\r\nabccccccccc",
			test2_body_message
		),
		TestCase(
			"GET / HTTP/1.1\r\nHost: test\r\nContent-Length: dddd\r\n\r\nabccccccccc",
			test3_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\n\r\n4\r\nWiki\r\n5\r\npedia\r\n0x34\r\n is a free online encyclopedia that "
			"anyone can edit.\r\n0\r\n\r\n",
			test4_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\n\r\n4\r\nWiki\r\n2\r\npedia\r\n0\r\n\r\n",
			test5_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\nContent-Length: 10\r\n\r\n4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n",
			test6_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\n\r\n4\r\nWiki\r\n5\r\npedia\r\n",
			test7_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\n\r\n4\r\nWiki\r\nss\r\npedia\r\n",
			test8_body_message
		),
		TestCase(
			"POST / HTTP/1.1\r\nHost: host\r\nTransfer-Encoding: "
			"chunked\r\n\r\n4\r\nWiki\r\n-122\r\npedia\r\n",
			test9_body_message
		),
	};

	ret_code |= RunTestCases(
		test_case_http_request_body_message_format,
		sizeof(test_case_http_request_body_message_format) /
			sizeof(test_case_http_request_body_message_format[0])
	);

	return ret_code;
}
