#include <cstring> // use strcpy
#include <iostream>
#include <map>

std::map<std::string, std::string> create_request_meta_variables();

const char **create_cgi_env(const std::map<std::string, std::string> &request_meta_variables) {
	const char **cgi_env = new const char *[request_meta_variables.size() + 1];
	size_t       i       = 0;
	for (std::map<std::string, std::string>::const_iterator ite = request_meta_variables.begin();
		 ite != request_meta_variables.end();
		 ite++) {
		const std::string element = ite->first + "=" + ite->second;
		cgi_env[i]                = new char[element.size() + 1];
		std::strcpy(const_cast<char *>(cgi_env[i]), element.c_str());
		i++;
	}
	cgi_env[i] = NULL;
	return (cgi_env);
}
