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
	// ? ステータスコードがOK以外の場合はCreateErrorResponseResult(data.request_result)
	// todo: VirtualServerクラス？？を引数で受けとり、リクエスト情報が正しいかどうかを確認する。
	// HttpResponseParsedData data = HttpResponseParse(const HttpRequestParsedData& data,
	// VirtualServer server);
	HttpResponseResult response = HttpResponse::CreateHttpResponseResult(data.request_result);
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
StatusCode TmpHttp::GetStatusCode(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.request_result.status_code;
}

bool TmpHttp::GetIsRequestLineFormat(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.is_request_format.is_request_line;
}

bool TmpHttp::GetIsHeaderFieldsFormat(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.is_request_format.is_header_fields;
}

bool TmpHttp::GetIsBodyMessageFormat(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.is_request_format.is_body_message;
}

std::string TmpHttp::GetBodyMessage(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.request_result.request.body_message;
}

} // namespace http
