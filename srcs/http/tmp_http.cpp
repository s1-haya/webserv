#include "tmp_http.hpp"
#include "http_message.hpp"
#include "http_result.hpp"
#include "http_storage.hpp"
#include <iostream>

namespace http {

TmpHttp::TmpHttp() {}

TmpHttp::~TmpHttp() {}

HttpResult TmpHttp::Run(
	const server::ClientInfo &client_info,
	const server::ServerInfo &server_info,
	const std::string        &read_buf
) {
	//todo: when HttpResponse::Run arguments require server_info.
	(void)server_info;
	HttpResult result;
	utils::Result<int> parsed_result = TmpParseHttpRequestFormat(client_info.GetFd(), read_buf);
	if (!parsed_result.IsOk()) {
		result.response      = CreateHttpResponse(client_info.GetFd());
		result.is_response_complete = true;
		return result;
	}
	if (GetIsHttpRequestFormatComplete(client_info.GetFd())) {
		result.response = CreateHttpResponse(client_info.GetFd());
		result.is_response_complete = true;
	}
	return result;
}

utils::Result<int> TmpHttp::TmpParseHttpRequestFormat(int client_fd, const std::string &read_buf) {
	utils::Result<int>    result;
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	save_data.current_buf += read_buf;
	try {
		HttpParse::TmpRunHttpResultVersion(save_data);
	} catch (const HttpParse::HttpParseException &e) {
		save_data.request_result.status_code = e.GetStatusCode();
		result.Set(false);
	}
	storage_.UpdateClientSaveData(client_fd, save_data);
	return result;
}

// HttpResult CreateTimeoutRequest(client_fd) {
// HttpResult result;
// HttpRequestParsedData data = storage_.GetClientSaveData(client_fd);
// result.is_response_complete = true;
// result.response = HttpResponse::CreateTimeoutRequest(data.request_result);
// result.request_buf = data.request_buf;
// todo: HttpResponse::IsConnectionKeep
// result.is_connection_keep = ;
// storage_.DeleteClientSaveData(client_fd);
// return result;
// }

// HttpResult CreateInternalServerError(client_fd)
// HttpResult result;
// HttpRequestParsedData data = storage_.GetClientSaveData(client_fd);
// result.is_response_complete = true;
// result.response = HttpResponse::CreateInternalServerError(data.request_result);;
// result.request_buf = data.request_buf;
// todo: HttpResponse::IsConnectionKeep
// result.is_connection_keep = ;
// storage_.DeleteClientSaveData(client_fd);
// return result;
// }

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
	storage_.DeleteClientSaveData(client_fd);
	return HttpResponse::Run(data.request_result);
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
