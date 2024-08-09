#include "config.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include <iostream>
#include <sstream>

namespace config {

const Config *Config::s_cInstance = NULL;

Config::Config(const std::string &file_path) : config_file_(file_path.c_str()) {
	if (!config_file_) {
		throw std::runtime_error("Cannot open Configuration file");
	}
	std::stringstream buffer;
	buffer << config_file_.rdbuf();
	std::list<node::Node> tokens;
	lexer::Lexer          lex(buffer.str(), tokens);
	parser::Parser        par(tokens);
	if (tokens.size() == 0) {
		throw std::runtime_error("No file content");
	}
	servers_ = par.GetServers();
	// try catchをどこでするか
}

Config::~Config() {}

const Config *Config::GetInstance() {
	return s_cInstance;
}

void Config::Create(const std::string &file_path) {
	if (!s_cInstance) {
		s_cInstance = new Config(file_path);
	}
}

void Config::Destroy() {
	delete s_cInstance;
	s_cInstance = NULL;
}

} // namespace config
