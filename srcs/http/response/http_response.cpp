#include "http_response.hpp"
#include "http_message.hpp"
#include <iostream>
#include <sstream>

namespace http {

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
	for (It it = response.header_fields.begin(); it != response.header_fields.end(); it++) {
		response_stream << it->first << ":" << SP << it->second << CRLF;
	}
	response_stream << CRLF;
	response_stream << response.body_message;
	return response_stream.str();
}

} // namespace http
