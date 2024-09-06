#include "http_exception.hpp"
#include "http_message.hpp"
#include "http_response.hpp"
#include "http_method.hpp"
#include "http_serverinfo_check.hpp"
#include "stat.hpp"
#include "system_exception.hpp"
#include <algorithm> // std::find
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h> // access

namespace {

bool IsExistPath(const std::string &path) {
	return access(path.c_str(), F_OK) == 0;
}

std::string FileToString(const std::ifstream &file) {
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string ReadFile(const std::string &file_path) {
	std::ifstream file(file_path.c_str());
	if (!file) {
		// todo: default error page?
		std::ifstream error_file("html/404.html");
		return FileToString(error_file);
	}
	return FileToString(file);
}

} // namespace

namespace http {

StatusCode Method::Handler(
	const std::string            &path,
	const std::string            &method,
	const std::list<std::string> &allow_method,
	const std::string            &request_body_message,
	std::string                  &response_body_message
) {
	StatusCode status_code(OK);
	bool       is_allow_method = IsAllowedMethod(method, allow_method);
	if (!is_allow_method) {
		throw HttpException("Error: Not Implemented", StatusCode(NOT_IMPLEMENTED));
	}
	else if (method == GET) {
		status_code = GetHandler(path, response_body_message);
	} else if (method == POST) {
		status_code = PostHandler(path, request_body_message, response_body_message);
	} else if (method == DELETE) {
		status_code = DeleteHandler(path, response_body_message);
	}
	return status_code;
}

// todo: refactor
StatusCode Method::GetHandler(const std::string &path, std::string &response_body_message) {
	StatusCode status_code(OK);
	Stat       info = TryStat(path, response_body_message);
	if (info.IsDirectory()) {
		// No empty string because the path has '/'
		if (path[path.size() - 1] != '/') {
			throw HttpException("Error: Moved Permanently", StatusCode(MOVED_PERMANENTLY));
		}
		// todo: Check for index directive and handle ReadFile function
		// todo: Check for autoindex directive and handle AutoindexHandler function
		// todo: Return 403 Forbidden if neither index nor autoindex directives exist
	} else if (info.IsRegularFile()) {
		if (!info.IsReadableFile()) {
			throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
		} else {
			response_body_message = ReadFile(path);
		}
	} else {
		throw HttpException("Error: Not Found", StatusCode(NOT_FOUND));
	}
	return status_code;
}

StatusCode Method::PostHandler(
	const std::string &path,
	const std::string &request_body_message,
	std::string       &response_body_message
) {
	if (!IsExistPath(path)) {
		return FileCreationHandler(path, request_body_message, response_body_message);
	}
	const Stat &info = TryStat(path, response_body_message);
	StatusCode  status_code(NO_CONTENT);
	if (info.IsDirectory()) {
		throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
	} else if (info.IsRegularFile()) {
		response_body_message = HttpResponse::CreateDefaultBodyMessageFormat(status_code);
	} else {
		// Location header fields: URI-reference
		// ex) POST /save/test.txt HTTP/1.1
		// Location: /save/test.txt;
		status_code = FileCreationHandler(path, request_body_message, response_body_message);
	}
	return status_code;
}

StatusCode
Method::DeleteHandler(const std::string &path, std::string &response_body_message) {
	const Stat &info        = TryStat(path, response_body_message);
	StatusCode  status_code = StatusCode(NO_CONTENT);
	if (info.IsDirectory()) {
		throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
	} else if (std::remove(path.c_str()) == 0) {
		response_body_message = HttpResponse::CreateDefaultBodyMessageFormat(status_code);
	} else {
		throw utils::SystemException(std::strerror(errno), errno);
	}
	return status_code;
}

void Method::SystemExceptionHandler(
	const utils::SystemException &e, std::string &response_body_message
) {
	int        error_number = e.GetErrorNumber();
	if (error_number == EACCES || error_number == EPERM) {
		throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
	} else if (error_number == ENOENT || error_number == ENOTDIR || error_number == ELOOP ||
			   error_number == ENAMETOOLONG) {
		throw HttpException("Error: Not Found", StatusCode(NOT_FOUND));
	} else {
		throw HttpException("Error: Internal Server Error", StatusCode(INTERNAL_SERVER_ERROR));
	}
}

StatusCode Method::FileCreationHandler(
	const std::string &path,
	const std::string &request_body_message,
	std::string       &response_body_message
) {
	StatusCode    status_code(CREATED);
	std::ofstream file(path.c_str(), std::ios::binary);
	if (file.fail()) {
		throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
	}
	file.write(request_body_message.c_str(), request_body_message.length());
	if (file.fail()) {
		file.close();
		if (std::remove(path.c_str()) != 0) {
			throw utils::SystemException(std::strerror(errno), errno);
		}
		throw HttpException("Error: Forbidden", StatusCode(FORBIDDEN));
	}
	response_body_message = HttpResponse::CreateDefaultBodyMessageFormat(status_code);
	return status_code;
}

Stat Method::TryStat(const std::string &path, std::string &response_body_message) {
	struct stat stat_buf;
	try {
		if (stat(path.c_str(), &stat_buf) == -1) {
			std::string error_message =
				"Error: stat on path '" + path + "': " + std::strerror(errno);
			throw utils::SystemException(error_message, errno);
		}
	} catch (const utils::SystemException &e) {
		SystemExceptionHandler(e, response_body_message);
	}
	Stat info(stat_buf);
	return info;
}

bool Method::IsAllowedMethod(
	const std::string &method, const std::list<std::string> &allow_method
) {
	if (allow_method.empty()) {
		// allow_methodがない場合はwebservが許可したメソッドのみ許可する（GETのみ）
		return std::find(
				   DEFAULT_ALLOWED_METHODS,
				   DEFAULT_ALLOWED_METHODS + DEFAULT_ALLOWED_METHODS_SIZE,
				   method
			   ) != DEFAULT_ALLOWED_METHODS + DEFAULT_ALLOWED_METHODS_SIZE;
	} else {
		return std::find(allow_method.begin(), allow_method.end(), method) != allow_method.end();
	}
}

} // namespace http