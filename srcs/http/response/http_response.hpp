#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include "cgi_response_parse.hpp"
#include "http_format.hpp"
#include "http_method.hpp"
#include "http_result.hpp"
#include "http_serverinfo_check.hpp"
#include "status_code.hpp"
#include "utils.hpp"
#include <map>
#include <string>

namespace cgi {

struct CgiResponse;

}

namespace http {

struct HttpRequestResult;
struct ClientInfos;
class Stat;

// HttpResponse {
// public:
//   static std::string Run(HttpRequestResult)
//   static std::string CreateTimeoutRequestResponse(HttpRequestResult)
//   static std::string CreateInternalServerErrorResponse(HttpRequestResult)
//  private:
//   static std::string        CreateHttpResponseFormat(const HttpResponseFormat &response);
//   static HttpResponseFormat CreateHttpResponseFormat(const HttpRequestResult &request_info);
//   static HttpResponseFormat CreateSuccessHttpResponseFormat(const HttpRequestResult
//   &request_info);
//   static HttpResponseFormat CreateDefaultHttpResponseFormat(const HttpRequestResult
//   &request_info);
//    GetTimeoutRequestBodyMessage();
//    GetInternalServerErrorBodyMessage();
// };

struct HttpResponseResult {
	HttpResponseResult(bool is_connection_close, const std::string &response)
		: is_connection_close(is_connection_close), response(response) {}
	bool        is_connection_close;
	std::string response;
};

struct HttpResponseFormatResult {
	HttpResponseFormatResult(
		bool is_connection_close, const HttpResponseFormat &http_response_format
	)
		: is_connection_close(is_connection_close), http_response_format(http_response_format) {}
	bool               is_connection_close;
	HttpResponseFormat http_response_format;
};

class HttpResponse {
  public:
	typedef std::map<EStatusCode, std::string> ReasonPhrase;
	static HttpResponseResult
					   Run(const http::ClientInfos             &client_info,
						   const server::VirtualServerAddrList &server_info,
						   const HttpRequestResult             &request_info,
						   CgiResult                           &cgi_result);
	static std::string CreateErrorResponse(const StatusCode &status_code);
	static bool        IsConnectionKeep(const HeaderFields &request_header_fields);
	static std::string CreateDefaultBodyMessage(const StatusCode &status_code);
	static std::string GetResponseFromCgi(
		const cgi::CgiResponseParse::ParsedData &cgi_parsed_data,
		const HttpRequestResult                 &request_info
	);

  private:
	HttpResponse();
	~HttpResponse();

	static std::string              CreateHttpResponse(const HttpResponseFormat &response);
	static HttpResponseFormatResult CreateHttpResponseFormat(
		const http::ClientInfos             &client_info,
		const server::VirtualServerAddrList &server_info,
		const HttpRequestResult             &request_info,
		CgiResult                           &cgi_result
	);
	static HeaderFields InitResponseHeaderFields(const HttpRequestResult &request_info);
	static bool         IsCgi(
				const std::string          &cgi_extension,
				const std::string          &path,
				const std::string          &method,
				const Method::AllowMethods &allowed_methods
			);
	static HttpResponseFormat HandleRedirect(
		HeaderFields &response_header_fields, const CheckServerInfoResult &server_info_result
	);
};

} // namespace http

#endif
