#include "Config.hpp"
#include "Server.hpp"
#include <cstdlib> // EXIT_
#include <iostream>
#include <string>

namespace {
	void PrintError(const std::string &s) {
		std::cerr << COLOR_RED "Error: " << s << COLOR_RESET << std::endl;
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
	Config::ConfigData config = Config::ParseConfig(path_config);

	try {
		Server server(config);
		server.Run();
	} catch (const std::exception &e) {
		PrintError(e.what());
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
