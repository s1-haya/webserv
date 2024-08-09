#include "http_response.hpp"
#include "http_message.hpp"
#include "utils.hpp"
#include <iostream>
#include <sstream>

namespace http {

// todo: StatusCodeクラスへ
HttpResponse::StatusReason InitStatusReason() {
	HttpResponse::StatusReason init_status_reason;
	init_status_reason[OK]              = "OK";
	init_status_reason[BAD_REQUEST]     = "Bad Request";
	init_status_reason[NOT_FOUND]       = "Not Found";
	init_status_reason[NOT_IMPLEMENTED] = "Not Implemented";
	return init_status_reason;
}

HttpResponse::StatusReason status_reason = InitStatusReason();

// todo:　HttpResponseParsedDataを元にHttpResponseResultを作成する
HttpResponseResult HttpResponse::CreateHttpResponseResult(const HttpRequestResult &request_info) {
	(void)request_info;
	HttpResponseResult response;
	response.status_line.version         = HTTP_VERSION;
	response.status_line.status_code     = "200";
	response.status_line.status_reason   = "OK";
	response.header_fields["Host"]       = "sawa";
	response.header_fields["Connection"] = "close";
	response.body_message = "You can't connect the dots looking forword. You can only connect the "
							"dots looking backwards";
	return response;
}

std::string HttpResponse::CreateErrorBodyMessage(
	const std::string &status_code, const std::string &status_reason
) {
	std::ostringstream body_message;
	body_message << "<html>" << CRLF << "<head><title>" << status_code << SP << status_reason
				 << "</title></head>" << CRLF << "<body>" << CRLF << "<center><h1>" << status_code
				 << SP << status_reason << "</h1></center>" << CRLF << "<hr><center>"
				 << SERVER_VERSION << "</center>" << CRLF << "</body>" << CRLF << "</html>" << CRLF;
	return body_message.str();
}

HttpResponseResult HttpResponse::CreateErrorHttpResponseResult(const HttpRequestResult &request_info
) {
	(void)request_info;
	HttpResponseResult response;
	response.status_line.version       = HTTP_VERSION;
	response.status_line.status_code   = utils::ToString(request_info.status_code);
	response.status_line.status_reason = status_reason[request_info.status_code];
	// todo: StatusCodeをクラスにして、プライベートで保持する。
	// response.status_line.status_code   = request_info.status_code.GetStrStatusCode();
	// response.status_line.status_reason   = request_info.status_code.GetStatusReason();
	response.header_fields["Content-Type"] = "text/html";
	response.header_fields["Server"]       = SERVER_VERSION;
	response.header_fields["Connection"]   = "close";
	response.body_message                  = CreateErrorBodyMessage(
        response.status_line.status_code, response.status_line.status_reason
    );
	response.header_fields["Content-Length"] = utils::ToString(response.body_message.length());
	return response;
}

// todo: CheckValidRequest:仮想サーバーとロケーションのディレクトリからリクエストの有効性を確認する
// HttpResponseParsedData HttpResponse::CheckValidRequest(const HttpRequestResult &request_info,
// const SeverConfig& server_config_info) {
// - リソースがサーバー上に存在しない
// - ファイルのアクセス権限ない
// - メソッドが許可されてない
// todo: cgiをリクエストしている場合は実行する。条件は以下だ。
// - サーバーのディレクトリにcgi拡張子ファイルを許可するディレクティブがある場合
// - リソースが上記のサーバーのディレクトリの場合
// - メソッドがGET, POSTかつそれらが許可されてる場合
// }

// HttpResponseResult構造体をHttpResponseのフォーマットを文字列に出力する
std::string HttpResponse::CreateHttpResponse(const HttpResponseResult &response) {
	std::ostringstream response_stream;
	response_stream << response.status_line.version << SP << response.status_line.status_code << SP
					<< response.status_line.status_reason << CRLF;
	typedef HeaderFields::const_iterator It;
	for (It it = response.header_fields.begin(); it != response.header_fields.end(); ++it) {
		response_stream << it->first << ":" << SP << it->second << CRLF;
	}
	response_stream << CRLF;
	response_stream << response.body_message;
	return response_stream.str();
}

} // namespace http
