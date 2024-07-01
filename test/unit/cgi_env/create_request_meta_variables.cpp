#include <iostream>
#include <map>

std::map<std::string, std::string> create_request_meta_variables() {
	std::map<std::string, std::string> request_meta_variables;
	request_meta_variables["AUTH_TYPE"];
	request_meta_variables["CONTENT_LENGTH"];
	request_meta_variables["CONTENT_TYPE"];
	request_meta_variables["GATEWAY_INTERFACE"] = "CGI/1.1";
	request_meta_variables["PATH_INFO"];
	request_meta_variables["PATH_TRANSLATED"];
	request_meta_variables["QUERY_STRING"];
	request_meta_variables["REMOTE_ADDR"];
	request_meta_variables["REMOTE_HOST"];
	request_meta_variables["REMOTE_IDENT"];
	request_meta_variables["REMOTE_USER"];
	request_meta_variables["REQUEST_METHOD"];
	request_meta_variables["SCRIPT_NAME"]     = "/cgi-bin/print_env.pl";
	request_meta_variables["SERVER_NAME"]     = "localhost";
	request_meta_variables["SERVER_PORT"]     = "8080";
	request_meta_variables["SERVER_PROTOCOL"] = "HTTP/1.1";
	request_meta_variables["SERVER_SOFTWARE"] = "Webserv/1.1";
	return (request_meta_variables);
}

// int main(void) {
// 	std::map<std::string, std::string> request_meta_variables = create_request_meta_variables();
// 	std::cout << request_meta_variables["HELLO"] << std::endl;
// 	std::cout << request_meta_variables["SERVER_NAME"] << std::endl;
// 	return (0);
// }
