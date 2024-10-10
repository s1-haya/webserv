#include "http_exception.hpp"
#include "utils.hpp"
#include <iostream>

namespace http {

HttpException::HttpException(const std::string &error_message, const StatusCode &status_code)
	: runtime_error(error_message), status_code_(status_code) {
	std::cerr << utils::color::GRAY << "[HttpException] " << status_code.GetEStatusCode() << ": "
			  << error_message << utils::color::RESET << std::endl;
}

HttpException::~HttpException() throw() {}

const StatusCode &HttpException::GetStatusCode() const throw() {
	return status_code_;
}

} // namespace http
