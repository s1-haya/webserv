#include "tmp_http.hpp"
#include "http_message.hpp"
#include <iostream>

namespace http {

TmpHttp::TmpHttp() {}

TmpHttp::~TmpHttp() {}

// todo: クライアントのリクエスト情報を読み込む
void TmpHttp::ParseHttpRequestFormat(int client_fd, const std::string &read_buf) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	save_data.current_buf += read_buf;
	HttpParse::TmpRun(save_data);
	storage_.UpdateClientSaveData(client_fd, save_data);
}

// todo: レスポンスを作成する
std::string TmpHttp::CreateHttpResponse(int client_fd) {
	HttpRequestParsedData data = storage_.GetClientSaveData(client_fd);
	HttpResponseResult    response;
	if (data.request_result.status_code == OK) {
		// todo: リクエスト情報が正しいかどうかを確認する。
		// HttpResponseParsedData data = HttpResponseParse(const HttpRequestParsedData& data,
		// VirtualServer server);
		// todo: 引数はHttpResponseParsedDataになる。
		response = HttpResponse::CreateHttpResponseResult(data.request_result);
	} else {
		// todo: ステータスコードがOK以外の場合はCreateErrorResponseResult(data.request_result)
		response = HttpResponse::CreateErrorHttpResponseResult(data.request_result);
	}
	storage_.DeleteClientSaveData(client_fd);
	return HttpResponse::CreateHttpResponse(response);
}

// todo: HTTPRequestの書式が完全かどうか(どのように取得するかは要検討)
bool TmpHttp::GetIsHttpRequestFormatComplete(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.is_request_format.is_request_line &&
		   save_data.is_request_format.is_header_fields &&
		   save_data.is_request_format.is_body_message;
}

// For test
HttpRequestParsedData TmpHttp::GetClientData(int client_fd) {
	return storage_.GetClientSaveData(client_fd);
}

} // namespace http
