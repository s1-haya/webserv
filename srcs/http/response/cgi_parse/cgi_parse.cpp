#include "cgi_parse.hpp"

namespace http {
namespace cgi {
namespace {

std::string CreatePathInfo(const std::string &cgi_extension, const std::string &request_target) {
	std::string::size_type pos = request_target.find(cgi_extension);
	if (pos != std::string::npos) {
		return request_target.substr(pos + cgi_extension.length());
	}
	return "";
}

std::string
TranslateToCgiPath(const std::string &cgi_extension, const std::string &request_target) {
	// from cgi_parse dir to cgi dir
	std::string::size_type pos = request_target.find(cgi_extension);
	if (pos != std::string::npos) { // for /aa.cgi/bb only return /aa.cgi
		return "../../../../cgi-bin" + request_target.substr(0, pos + cgi_extension.length());
	}
	return "../../../../cgi-bin" + request_target;
}

std::string TranslateToHtmlPath(const std::string &request_target) {
	// from cgi_parse dir to html dir
	return "../../../../html" + request_target;
}

} // namespace

const std::string AUTH_TYPE         = "AUTH_TYPE";
const std::string CONTENT_LENGTH    = "CONTENT_LENGTH";
const std::string CONTENT_TYPE      = "CONTENT_TYPE";
const std::string GATEWAY_INTERFACE = "GATEWAY_INTERFACE";
const std::string PATH_INFO         = "PATH_INFO";
const std::string PATH_TRANSLATED   = "PATH_TRANSLATED";
const std::string QUERY_STRING      = "QUERY_STRING";
const std::string REMOTE_ADDR       = "REMOTE_ADDR";
const std::string REMOTE_HOST       = "REMOTE_HOST";
const std::string REMOTE_IDENT      = "REMOTE_IDENT";
const std::string REMOTE_USER       = "REMOTE_USER";
const std::string REQUEST_METHOD    = "REQUEST_METHOD";
const std::string SCRIPT_NAME       = "SCRIPT_NAME";
const std::string SERVER_NAME       = "SERVER_NAME";
const std::string SERVER_PORT       = "SERVER_PORT";
const std::string SERVER_PROTOCOL   = "SERVER_PROTOCOL";
const std::string SERVER_SOFTWARE   = "SERVER_SOFTWARE";

MetaMap CgiParse::CreateRequestMetaVariables(
	const HttpRequestFormat &request,
	const std::string       &cgi_script,
	const std::string       &cgi_extension,
	const std::string       &server_port
) {
	MetaMap request_meta_variables;
	request_meta_variables[AUTH_TYPE]         = "";
	request_meta_variables[CONTENT_LENGTH]    = request.header_fields.at("Content-Length");
	request_meta_variables[CONTENT_TYPE]      = request.header_fields.at("Content-Type");
	request_meta_variables[GATEWAY_INTERFACE] = "CGI/1.1";
	request_meta_variables[PATH_INFO]         = CreatePathInfo(cgi_extension, cgi_script);
	request_meta_variables[PATH_TRANSLATED] =
		TranslateToHtmlPath(request_meta_variables["PATH_INFO"]);
	request_meta_variables[QUERY_STRING]    = "";
	request_meta_variables[REMOTE_ADDR]     = ""; // 追加する？
	request_meta_variables[REMOTE_HOST]     = "";
	request_meta_variables[REMOTE_IDENT]    = "";
	request_meta_variables[REMOTE_USER]     = "";
	request_meta_variables[REQUEST_METHOD]  = request.request_line.method;
	request_meta_variables[SCRIPT_NAME]     = TranslateToCgiPath(cgi_extension, cgi_script);
	request_meta_variables[SERVER_NAME]     = request.header_fields.at("Host");
	request_meta_variables[SERVER_PORT]     = server_port;
	request_meta_variables[SERVER_PROTOCOL] = request.request_line.version;
	request_meta_variables[SERVER_SOFTWARE] = "Webserv/1.1";
	return request_meta_variables;
}

utils::Result<CgiRequest> CgiParse::Parse(
	const HttpRequestFormat &request,
	const std::string       &cgi_script,
	const std::string       &cgi_extension,
	const std::string       &server_port
) {
	CgiRequest                cgi_request;
	utils::Result<CgiRequest> result;

	try {
		cgi_request.meta_variables =
			CreateRequestMetaVariables(request, cgi_script, cgi_extension, server_port);
		cgi_request.body_message = request.body_message;
		result.SetValue(cgi_request);
	} catch (const std::exception &e) {
		result.Set(false); // atで例外が投げられる
	}
	return result;
}

} // namespace cgi
} // namespace http