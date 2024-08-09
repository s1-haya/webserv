#ifndef PARSER_HPP_
#define PARSER_HPP_

#include "context.hpp"
#include "node.hpp"
#include <list>

namespace config {
namespace parser {

typedef std::list<node::Node>::iterator NodeItr;

class Parser {
  private:
	std::list<node::Node>        &tokens_;
	std::list<context::ServerCon> servers_;
	// Prohibit Copy
	Parser(const Parser &);
	Parser &operator=(const Parser &);

	context::ServerCon   CreateServerContext(NodeItr &);
	context::LocationCon CreateLocationContext(NodeItr &);

	void HandleServerContextDirective(context::ServerCon &server, NodeItr &it);
	void HandleLocationContextDirective(context::LocationCon &location, NodeItr &it);

  public:
	Parser(std::list<node::Node> &);
	~Parser();

	std::list<context::ServerCon> GetServers() const;
};

} // namespace parser
} // namespace config

#endif
