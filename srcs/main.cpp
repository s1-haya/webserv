#include "_config.hpp"
#include "color.hpp"
#include "server.hpp"
#include <cstdlib> // EXIT_
#include <iostream>
#include <string>

namespace {

void PrintError(const std::string &s) {
	std::cerr << utils::color::RED << "Error: " << s << utils::color::RESET << std::endl;
}

} // namespace

// ./webserv [configuration file]
int main(int argc, char **argv) {
	if (argc > 2) {
		PrintError("invalid arguments");
		return EXIT_FAILURE;
	}

	// todo: handle parse error
	std::string path_config;
	if (argc == 1) {
		path_config = "default.conf";
	} else {
		path_config = std::string(argv[1]);
	}
	_config::Config::ConfigData config = _config::Config::ParseConfig(path_config);

	try {
		server::Server server(config);
		server.Init();
		server.Run();
	} catch (const std::exception &e) {
		PrintError(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
