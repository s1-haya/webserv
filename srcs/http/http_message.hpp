#ifndef HTTP_MESSAGE_HPP_
#define HTTP_MESSAGE_HPP_

#include <string>

namespace http {

extern const std::string SP;
extern const std::string HTAB;
extern const std::string OPTIONAL_WHITESPACE;
extern const std::string CRLF;
extern const std::string HTTP_VERSION;
extern const std::string SERVER_VERSION;

extern const std::string GET;
extern const std::string DELETE;
extern const std::string POST;
extern const std::string BASIC_METHODS[];
extern const std::size_t BASIC_METHODS_SIZE;

extern const std::string HOST;
extern const std::string USER_AGENT;
extern const std::string ACCEPT;
extern const std::string ACCEPT_ENCODING;
extern const std::string CONNECTION;
extern const std::string CONTENT_TYPE;
extern const std::string CONTENT_LENGTH;
extern const std::string TRANSFER_ENCODING;
extern const std::string AUTHORIZATION;
extern const std::string BASIC_HEADER_FIELDS[];
extern const std::size_t BASIC_HEADER_FIELDS_SIZE;

} // namespace http

#endif /* HTTP_MESSAGE_HPP_ */
