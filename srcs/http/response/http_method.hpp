#ifndef HTTP_METHOD_HPP_
#define HTTP_METHOD_HPP_

#include "stat.hpp"
#include "status_code.hpp"
#include <list>

namespace utils {
class SystemException;
}

namespace http {

class Method {
  public:
	static StatusCode Handler(
		const std::string            &path,
		const std::string            &method,
		const std::list<std::string> &allow_method,
		const std::string            &request_body_message,
		std::string                  &response_body_message
	);
	static StatusCode GetHandler(const std::string &path, std::string &body_message);
	static StatusCode PostHandler(
		const std::string &path,
		const std::string &request_body_message,
		std::string       &response_body_message
	);
	static StatusCode DeleteHandler(const std::string &path, std::string &response_body_message);

  private:
	static Stat TryStat(const std::string &path, std::string &response_body_message);
	static bool
	IsAllowedMethod(const std::string &method, const std::list<std::string> &allow_method);
	static void
	SystemExceptionHandler(const utils::SystemException &e, std::string &response_body_message);
	static StatusCode FileCreationHandler(
		const std::string &path,
		const std::string &request_body_message,
		std::string       &response_body_message
	);
};

} // namespace http

#endif
