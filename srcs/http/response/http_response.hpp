#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include "http_parse.hpp"
#include <string>

namespace http {

struct StatusLine {
	std::string version;
	std::string status_code;
	std::string status_reason;
};

struct HttpResponseResult {
	StatusLine   status_line;
	HeaderFields header_fields;
	std::string  body_message;
};

class HttpResponse {
  public:
	static std::string        CreateHttpResponse(const HttpResponseResult &response);
	static HttpResponseResult CreateHttpResponseResult(const HttpRequestResult &request_info);

  private:
	HttpResponse();
	~HttpResponse();
};

} // namespace http

#endif
