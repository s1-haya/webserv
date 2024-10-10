#include "http_response.hpp"
#include "cgi.hpp"
#include "cgi_parse.hpp"
#include "client_infos.hpp"
#include "http_exception.hpp"
#include "http_message.hpp"
#include "http_parse.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace http {
namespace {

std::string GetPathAfterRoot(const std::string &path) {
	const std::string root = "/root";
	size_t            pos  = path.find(root);
	if (pos != std::string::npos) {
		return path.substr(pos + root.length());
	}
	return path;
}

std::string GetCwd() {
	const char            *file_path = __FILE__;
	std::string            path(file_path);
	std::string::size_type pos       = path.find_last_of("/\\");
	std::string            directory = (pos != std::string::npos) ? path.substr(0, pos) : "";
	return directory;
}

std::string ReadErrorFile(const std::string &file_path) {
	const std::string root_path = GetCwd() + "/../../../root";
	std::ifstream     file((root_path + file_path).c_str());
	if (!file) {
		if (errno == EACCES || errno == EPERM) {
			return HttpResponse::CreateDefaultBodyMessage(StatusCode(FORBIDDEN));
		} else if (errno == ENOENT || errno == ENOTDIR || errno == ELOOP || errno == ENAMETOOLONG) {
			return HttpResponse::CreateDefaultBodyMessage(StatusCode(NOT_FOUND));
		} else {
			return HttpResponse::CreateDefaultBodyMessage(StatusCode(INTERNAL_SERVER_ERROR));
		}
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

bool IsErrorConnectionClose(EStatusCode status_code) {
	return status_code == http::BAD_REQUEST || status_code == http::INTERNAL_SERVER_ERROR;
}

void SetErrorConnectionClose(HeaderFields &header_fields, EStatusCode status_code) {
	if (IsErrorConnectionClose(status_code)) {
		header_fields[CONNECTION] = CLOSE;
	}
}

} // namespace

HttpResponseResult HttpResponse::Run(
	const http::ClientInfos             &client_info,
	const server::VirtualServerAddrList &server_info,
	const HttpRequestResult             &request_info,
	CgiResult                           &cgi_result
) {
	HttpResponseFormatResult response_format_result =
		CreateHttpResponseFormat(client_info, server_info, request_info, cgi_result);
	if (cgi_result.is_cgi) {
		return HttpResponseResult(false, "");
	}
	return HttpResponseResult(
		response_format_result.is_connection_close,
		CreateHttpResponse(response_format_result.http_response_format)
	);
}

HttpResponseFormatResult HttpResponse::CreateHttpResponseFormat(
	const http::ClientInfos             &client_info,
	const server::VirtualServerAddrList &server_info,
	const HttpRequestResult             &request_info,
	CgiResult                           &cgi_result
) {
	StatusCode   status_code(OK);
	HeaderFields response_header_fields = InitResponseHeaderFields(request_info);
	std::string  response_body_message;
	utils::Result< std::pair<unsigned int, std::string> > error_page;

	try {
		const CheckServerInfoResult &server_info_result =
			HttpServerInfoCheck::Check(server_info, request_info.request);
		error_page = server_info_result.error_page;
		if (server_info_result.redirect.IsOk()) {
			// redirect時のエラーでconnection:closeで返したい場合はcatchに入ってくれる
			return HttpResponseFormatResult(
				false, HandleRedirect(response_header_fields, server_info_result)
			);
		}
		if (IsCgi(
				server_info_result.cgi_extension,
				server_info_result.path,
				request_info.request.request_line.method,
				server_info_result.allowed_methods
			)) {
			// これはパースした結果
			cgi::CgiRequest cgi_request = CgiParse::Parse(
				request_info.request,
				server_info_result.path,
				server_info_result.cgi_extension,
				utils::ToString(client_info.listen_server_port),
				client_info.ip
			);
			cgi_result.is_cgi      = true;
			cgi_result.cgi_request = cgi_request;
		} else {
			status_code = Method::Handler(
				server_info_result.path,
				request_info.request.request_line.method,
				server_info_result.allowed_methods,
				request_info.request.body_message,
				request_info.request.header_fields,
				response_body_message,
				response_header_fields,
				server_info_result.index,
				server_info_result.autoindex,
				server_info_result.file_upload_path
			);
		}
	} catch (const HttpException &e) {
		// ステータスコードが300番台以上の場合
		// for debug
		std::cerr << utils::color::GRAY << "Debug [" << e.what() << "]" << utils::color::RESET
				  << std::endl;

		status_code = e.GetStatusCode();
		if (error_page.IsOk() && status_code.GetEStatusCode() == error_page.GetValue().first) {
			utils::Debug("ErrorPage", error_page.GetValue().second);
			response_body_message = ReadErrorFile(error_page.GetValue().second);
		} else {
			response_body_message = CreateDefaultBodyMessage(status_code);
		}
	}
	response_header_fields[CONTENT_LENGTH] = utils::ToString(response_body_message.length());
	SetErrorConnectionClose(response_header_fields, status_code.GetEStatusCode());
	return HttpResponseFormatResult(
		IsErrorConnectionClose(status_code.GetEStatusCode()),
		HttpResponseFormat(
			StatusLine(HTTP_VERSION, status_code.GetStatusCode(), status_code.GetReasonPhrase()),
			response_header_fields,
			response_body_message
		)
	);
}

std::string HttpResponse::CreateDefaultBodyMessage(const StatusCode &status_code) {
	std::ostringstream body_message;
	body_message << "<html>" << CRLF << "<head><title>" << status_code.GetStatusCode() << SP
				 << status_code.GetReasonPhrase() << "</title></head>" << CRLF << "<body>" << CRLF
				 << "<center><h1>" << status_code.GetStatusCode() << SP
				 << status_code.GetReasonPhrase() << "</h1></center>" << CRLF << "<hr><center>"
				 << SERVER_VERSION << "</center>" << CRLF << "</body>" << CRLF << "</html>" << CRLF;
	return body_message.str();
}

std::string HttpResponse::CreateHttpResponse(const HttpResponseFormat &response) {
	std::ostringstream response_stream;
	response_stream << response.status_line.version << SP << response.status_line.status_code << SP
					<< response.status_line.reason_phrase << CRLF;
	typedef HeaderFields::const_iterator It;
	for (It it = response.header_fields.begin(); it != response.header_fields.end(); ++it) {
		response_stream << it->first << ":" << SP << it->second << CRLF;
	}
	response_stream << CRLF;
	response_stream << response.body_message;
	return response_stream.str();
}

HeaderFields HttpResponse::InitResponseHeaderFields(const HttpRequestResult &request_info) {
	HeaderFields response_header_fields;
	response_header_fields[SERVER]       = SERVER_VERSION;
	response_header_fields[CONTENT_TYPE] = TEXT_HTML;
	if (IsConnectionKeep(request_info.request.header_fields)) {
		response_header_fields[CONNECTION] = KEEP_ALIVE;
	} else {
		response_header_fields[CONNECTION] = CLOSE;
	}
	return response_header_fields;
}

bool HttpResponse::IsConnectionKeep(const HeaderFields &request_header_fields) {
	HeaderFields::const_iterator it = request_header_fields.find(CONNECTION);
	return it == request_header_fields.end() || it->second != CLOSE;
}

bool HttpResponse::IsCgi(
	const std::string          &cgi_extension,
	const std::string          &path,
	const std::string          &method,
	const Method::AllowMethods &allowed_methods
) {
	// cgi_extensionがあるかどうか
	if (cgi_extension.empty()) {
		return false;
	}
	// pathにcgi_extensionで設定された拡張子が含まれているかどうか
	// falseの場合はpathに普通のリクエストとして送られる
	if (GetPathAfterRoot(path).find(cgi_extension) == std::string::npos) {
		return false;
	}
	// methodがGETかPOSTかつallow_methodかどうか
	if (!(Method::IsAllowedMethod(method, allowed_methods)) || (method != GET && method != POST)) {
		return false;
	}
	return true;
}

HttpResponseFormat HttpResponse::HandleRedirect(
	HeaderFields &response_header_fields, const CheckServerInfoResult &server_info_result
) {
	// httpがない場合は現在のサーバーに送り、httpがある場合はそのまま
	const std::string prefix = "http://";
	const std::string url    = server_info_result.redirect.GetValue().second;
	if (url.compare(0, prefix.size(), prefix) == 0) {
		response_header_fields[LOCATION] = server_info_result.redirect.GetValue().second;
	} else {
		response_header_fields[LOCATION] = prefix + server_info_result.host_name + url;
	}
	utils::Debug("Redirect", response_header_fields[LOCATION]);
	// サーバーで用意しているステータス以外を指定した場合はbodyやphraseを返さない
	if (server_info_result.redirect.GetValue().first == MOVED_PERMANENTLY) {
		throw HttpException("Moved Permanently", StatusCode(MOVED_PERMANENTLY));
	} else if (server_info_result.redirect.GetValue().first == FOUND) {
		throw HttpException("Found", StatusCode(FOUND));
	} else {
		response_header_fields[CONTENT_LENGTH] = "0";
		const std::string redirect_status_code =
			utils::ToString(server_info_result.redirect.GetValue().first);
		return HttpResponseFormat(
			StatusLine(HTTP_VERSION, redirect_status_code, ""), response_header_fields, ""
		);
	}
}

std::string HttpResponse::CreateErrorResponse(const StatusCode &status_code) {
	HttpResponseFormat response;
	response.status_line =
		StatusLine(HTTP_VERSION, status_code.GetStatusCode(), status_code.GetReasonPhrase());
	response.header_fields[SERVER]         = SERVER_VERSION;
	response.header_fields[CONTENT_TYPE]   = TEXT_HTML;
	response.header_fields[CONNECTION]     = CLOSE;
	response.body_message                  = CreateDefaultBodyMessage(status_code);
	response.header_fields[CONTENT_LENGTH] = utils::ToString(response.body_message.length());
	return CreateHttpResponse(response);
}

std::string HttpResponse::GetResponseFromCgi(
	const cgi::CgiResponseParse::ParsedData &cgi_parsed_data, const HttpRequestResult &request_info
) {
	StatusCode  status_code(OK);
	std::string response_body_message = cgi_parsed_data.body;

	HeaderFields response_header_fields;
	response_header_fields[SERVER] = SERVER_VERSION;
	// Content-Tyeがない場合はapplication/octet-streamを設定
	if (cgi_parsed_data.header_fields.find(CONTENT_TYPE) == cgi_parsed_data.header_fields.end()) {
		response_header_fields[CONTENT_TYPE] = APPLICATION_OCTET_STREAM;
	} else {
		response_header_fields[CONTENT_TYPE] = cgi_parsed_data.header_fields.at(CONTENT_TYPE);
	}
	// Content-Lengthがない場合はbodyの長さを設定
	if (cgi_parsed_data.header_fields.find(CONTENT_LENGTH) == cgi_parsed_data.header_fields.end()) {
		response_header_fields[CONTENT_LENGTH] = utils::ToString(response_body_message.length());
	} else {
		response_header_fields[CONTENT_LENGTH] = cgi_parsed_data.header_fields.at(CONTENT_LENGTH);
	}
	if (IsConnectionKeep(request_info.request.header_fields)) {
		response_header_fields[CONNECTION] = KEEP_ALIVE;
	} else {
		response_header_fields[CONNECTION] = CLOSE;
	}
	if (cgi_parsed_data.header_fields.find(LOCATION) != cgi_parsed_data.header_fields.end()) {
		status_code                      = StatusCode(FOUND);
		response_header_fields[LOCATION] = cgi_parsed_data.header_fields.at(LOCATION);
	}

	HttpResponseFormat response_format(
		StatusLine(HTTP_VERSION, status_code.GetStatusCode(), status_code.GetReasonPhrase()),
		response_header_fields,
		response_body_message
	);

	return CreateHttpResponse(response_format);
}

} // namespace http
