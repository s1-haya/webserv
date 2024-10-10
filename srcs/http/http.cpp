#include "http.hpp"
#include "cgi_response_parse.hpp"
#include "client_infos.hpp"
#include "http_message.hpp"
#include "http_result.hpp"
#include "http_storage.hpp"
#include "status_code.hpp"
#include "utils.hpp"
#include <iostream>

namespace http {
namespace {

std::string CreateLocalRedirectRequest(const std::string &location, const std::string &host) {
	std::string response;
	response += "GET " + location + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
	return response;
}

bool IsConnectionKeep(bool is_connection_close, const HeaderFields &header_fields) {
	// responseのエラーの結果なので優先
	if (is_connection_close) {
		return false;
	}
	return HttpResponse::IsConnectionKeep(header_fields);
}

} // namespace

Http::Http() {}

Http::~Http() {}

HttpResult
Http::Run(const ClientInfos &client_info, const server::VirtualServerAddrList &server_info) {
	HttpResult          result;
	utils::Result<void> parsed_result =
		ParseHttpRequestFormat(client_info.fd, client_info.request_buf);
	if (!parsed_result.IsOk()) {
		return CreateBadRequestResponse(client_info.fd);
	}
	if (IsHttpRequestFormatComplete(client_info.fd)) {
		result = CreateHttpResponse(client_info, server_info);
	}
	return result;
}

HttpResult Http::GetResponseFromCgi(int client_fd, const cgi::CgiResponse &cgi_response) {
	typedef utils::Result<cgi::CgiResponseParse::ParsedData> CgiParseResult;
	HttpResult                                               result;
	CgiParseResult cgi_parse_result = cgi::CgiResponseParse::Parse(cgi_response.response);
	if (!cgi_parse_result.IsOk()) {
		return GetErrorResponse(client_fd, INTERNAL_ERROR);
	}
	HttpRequestParsedData               data          = storage_.GetClientSaveData(client_fd);
	cgi::CgiResponseParse::HeaderFields header_fields = cgi_parse_result.GetValue().header_fields;
	if (header_fields.find(LOCATION) != header_fields.end() &&
		utils::StartWith(header_fields[LOCATION], "/")) {
		// Hostがないリクエストヘッダはエラーで弾かれている
		result.request_buf =
			CreateLocalRedirectRequest(
				header_fields[LOCATION], data.request_result.request.header_fields.at(HOST)
			) +
			data.current_buf;
		result.is_response_complete = false;
	} else {
		result.request_buf          = data.current_buf;
		result.is_response_complete = true;
	}
	result.is_connection_keep =
		HttpResponse::IsConnectionKeep(data.request_result.request.header_fields);
	result.response =
		HttpResponse::GetResponseFromCgi(cgi_parse_result.GetValue(), data.request_result);
	storage_.DeleteClientSaveData(client_fd);
	return result;
}

utils::Result<void> Http::ParseHttpRequestFormat(int client_fd, const std::string &read_buf) {
	utils::Result<void>   result;
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	save_data.current_buf += read_buf;
	try {
		HttpParse::Run(save_data);
	} catch (const HttpException &e) {
		save_data.request_result.status_code = e.GetStatusCode();
		result.Set(false);
	}
	storage_.UpdateClientSaveData(client_fd, save_data);
	return result;
}

HttpResult Http::CreateHttpResponse(
	const ClientInfos &client_info, const server::VirtualServerAddrList &server_info
) {
	HttpResult            result;
	HttpRequestParsedData data = storage_.GetClientSaveData(client_info.fd);
	result.request_buf         = data.current_buf;
	HttpResponseResult response_result =
		HttpResponse::Run(client_info, server_info, data.request_result, result.cgi_result);
	result.is_connection_keep = IsConnectionKeep(
		response_result.is_connection_close, data.request_result.request.header_fields
	);
	result.response = response_result.response;
	// todo: 仮。CGI実行中はfalseにしたい
	result.is_response_complete = !result.cgi_result.is_cgi;
	if (!result.cgi_result.is_cgi) { // cgiの場合はcgiのhttp_responseを作るときにsave_dataが必要
		storage_.DeleteClientSaveData(client_info.fd);
	}
	return result;
}

HttpResult Http::GetErrorResponse(int client_fd, ErrorState state) {
	HttpResult            result;
	HttpRequestParsedData data  = storage_.GetClientSaveData(client_fd);
	result.is_response_complete = true;
	result.is_connection_keep   = false;
	result.request_buf          = data.current_buf;
	switch (state) {
	case TIMEOUT:
		result.response = HttpResponse::CreateErrorResponse(StatusCode(REQUEST_TIMEOUT));
		break;
	case INTERNAL_ERROR:
		result.response = HttpResponse::CreateErrorResponse(StatusCode(INTERNAL_SERVER_ERROR));
		break;
	default:
		break;
	}
	storage_.DeleteClientSaveData(client_fd);
	return result;
}

HttpResult Http::CreateBadRequestResponse(int client_fd) {
	HttpResult            result;
	HttpRequestParsedData data  = storage_.GetClientSaveData(client_fd);
	result.is_response_complete = true;
	result.is_connection_keep   = false;
	result.request_buf          = data.current_buf;
	result.response             = HttpResponse::CreateErrorResponse(StatusCode(BAD_REQUEST));
	storage_.DeleteClientSaveData(client_fd);
	return result;
}

// todo: HTTPRequestの書式が完全かどうか(どのように取得するかは要検討)
bool Http::IsHttpRequestFormatComplete(int client_fd) {
	HttpRequestParsedData save_data = storage_.GetClientSaveData(client_fd);
	return save_data.is_request_format.is_request_line &&
		   save_data.is_request_format.is_header_fields &&
		   save_data.is_request_format.is_body_message;
}

} // namespace http
