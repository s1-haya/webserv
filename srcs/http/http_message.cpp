#include "http_message.hpp"

namespace http {

const std::string SP                  = " ";
const std::string HTAB                = "\t";
const std::string OPTIONAL_WHITESPACE = SP + HTAB;
const std::string CRLF                = "\r\n";
const std::string HEADER_FIELDS_END   = CRLF + CRLF;
const std::string HTTP_VERSION        = "HTTP/1.1";
const std::string SERVER_VERSION      = "webserv/1.1";

const std::string GET                = "GET";
const std::string DELETE             = "DELETE";
const std::string POST               = "POST";
const std::string BASIC_METHODS[]    = {GET, DELETE, POST};
const std::size_t BASIC_METHODS_SIZE = sizeof(BASIC_METHODS) / sizeof(BASIC_METHODS[0]);

const std::string HOST                  = "Host";
const std::string USER_AGENT            = "User-Agent";
const std::string ACCEPT                = "Accept";
const std::string ACCEPT_ENCODING       = "Accept-Encoding";
const std::string CONNECTION            = "Connection";
const std::string CONTENT_TYPE          = "Content-Type";
const std::string CONTENT_LENGTH        = "Content-Length";
const std::string TRANSFER_ENCODING     = "Transfer-Encoding";
const std::string AUTHORIZATION         = "Authorization";
const std::string SERVER                = "Server";
const std::string BASIC_HEADER_FIELDS[] = {
	HOST,
	USER_AGENT,
	ACCEPT,
	ACCEPT_ENCODING,
	CONNECTION,
	CONTENT_TYPE,
	CONTENT_LENGTH,
	TRANSFER_ENCODING,
	AUTHORIZATION,
	SERVER
};
const std::size_t BASIC_HEADER_FIELDS_SIZE =
	sizeof(BASIC_HEADER_FIELDS) / sizeof(BASIC_HEADER_FIELDS[0]);

} // namespace http
