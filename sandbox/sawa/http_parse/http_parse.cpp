#include "http_parse.hpp"
#include "http_message.hpp"
#include "utils.hpp"
#include <algorithm> // std::find
#include <vector>

namespace http {
namespace {

bool IsUSASCII(const std::string &str) {
	typedef std::string::const_iterator It;
	for (It it = str.begin(); it != str.end(); it++) {
		if (static_cast<unsigned char>(*it) > 127) {
			return false;
		}
	}
	return true;
}

bool IsUpper(const std::string &str) {
	typedef std::string::const_iterator It;
	for (It it = str.begin(); it != str.end(); it++) {
		if (!std::isupper(static_cast<unsigned char>(*it))) {
			return false;
		}
	}
	return true;
}

// void PrintLines(const std::vector<std::string> &lines) {
// 	typedef std::vector<std::string>::const_iterator It;
// 	size_t                                           i = 0;
// 	for (It it = lines.begin(); it != lines.end(); ++it) {
// 		// if (*it == "") {
// 		// 	std::cout << "stop!" << std::endl;
// 		// 	break;
// 		// }
// 		std::cout << i << ":" << *it << std::endl;
// 		i++;
// 	}
// }

} // namespace

HttpParse::HttpParse() {}

HttpParse::~HttpParse() {}

std::string HttpParse::CheckMethod(const std::string &method) {
	// US-ASCIIかまたは大文字かどうか -> 400
	if (IsUSASCII(method) == false || IsUpper(method) == false)
		return "400";
	// GET, POST, DELETEかどうか ->　501
	std::vector<std::string> basic_methods;
	basic_methods.push_back("GET");
	basic_methods.push_back("POST");
	basic_methods.push_back("DELETE");
	if (std::find(basic_methods.begin(), basic_methods.end(), method) == basic_methods.end()) {
		return "501";
	}
	// 設定ファイルでメソッドが許可されてるかどうか -> 405
	// todo:
	// どの仮想サーバーのどのリソースがメソッド許可されてるかどうか？がリソースパースしていない段階ではわからないからこの関数で判定しないかも
	// if (allowed_methods.find(method) == allowed_methods.end()) {
	// 	return "405";
	// }
	return (method);
}

std::string HttpParse::CheckRequestTarget(const std::string &reqest_target) {
	// /が先頭になかったら場合 -> 400
	if (reqest_target.empty() || reqest_target[0] != '/')
		return "400";
	return reqest_target;
}

std::string HttpParse::CheckVersion(const std::string &version) {
	// HTTP/1.1かどうか -> 400
	if ("HTTP/1.1" != version)
		return ("400");
	return (version);
}

RequestLine HttpParse::SetRequestLine(const std::vector<std::string> &request_line_info) {
	// todo: 各値が正常な値かどうか確認してから作成する（エラーの場合はenumに設定？）
	RequestLine request_line(
		CheckMethod(request_line_info[0]),
		CheckRequestTarget(request_line_info[1]),
		CheckVersion(request_line_info[2])
	);
	return request_line;
}

HeaderFields HttpParse::SetHeaderFields(const std::vector<std::string> &header_fields_info) {
	// todo: 各値が正常な値かどうか確認してから作成する（エラーの場合はenumに設定？）
	HeaderFields                                     header_fields;
	typedef std::vector<std::string>::const_iterator It;
	for (It it = header_fields_info.begin() + 1; it != header_fields_info.end(); ++it) {
		std::vector<std::string> header_key_value = utils::SplitStr(*it, ": ");
		header_fields[header_key_value[0]]        = header_key_value[1];
	}
	return header_fields;
}

// todo: tmp request_
HttpRequestResult HttpParse::Run(const std::string &read_buf) {
	HttpRequestResult        result;
	// a: [request_line header_fields, messagebody]
	// b: [request_line, header_fields]
	std::vector<std::string> a = utils::SplitStr(read_buf, CRLF + CRLF);
	std::vector<std::string> b = utils::SplitStr(a[0], CRLF);
	result.request.request_line       = SetRequestLine(utils::SplitStr(b[0], SP));
	result.request.header_fields      = SetHeaderFields(b);
	// PrintLines(b);
	return result;
}

// status_line && header
// messagebody

} // namespace http