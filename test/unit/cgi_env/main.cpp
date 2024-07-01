#include <iostream>
#include <map>

std::map<std::string, std::string> create_request_meta_variables();
const char **create_cgi_env(const std::map<std::string, std::string> &request_meta_variables);

int main(void) {
	std::map<std::string, std::string> request_meta_variables = create_request_meta_variables();
	const char                       **cgi_env = create_cgi_env(request_meta_variables);
	for (size_t i = 0; cgi_env[i] != NULL; i++) {
		std::cout << cgi_env[i] << std::endl;
	}
	delete[] cgi_env;
	return (0);
}
