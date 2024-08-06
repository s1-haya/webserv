#ifndef CONTEXT_MANAGER_HPP_
#define CONTEXT_MANAGER_HPP_

#include "sock_context.hpp"
#include "virtual_server.hpp"
#include "virtual_server_storage.hpp"
#include <string>

namespace server {

struct DtoServerInfos {
	int                         server_fd;
	std::string                 server_name;
	std::string                 server_port;
	VirtualServer::LocationList locations;
};

// holds and manages virtual server info and socket context(server socket info, client socket info).
class ContextManager {
  public:
	ContextManager();
	~ContextManager();
	ContextManager(const ContextManager &other);
	ContextManager &operator=(const ContextManager &other);
	// functions
	void AddVirtualServer(const VirtualServer &virtual_server);
	void AddServerInfo(
		int server_fd, const ServerInfo &server_info, const VirtualServer *virtual_server
	);
	void AddClientInfo(int client_fd, const ClientInfo &client_info, int server_fd);
	void DeleteClientInfo(int client_fd);
	// getter
	const VirtualServerStorage::VirtualServerList &GetVirtualServerList() const;
	DtoServerInfos                                 GetServerInfo(int client_fd) const;
	const std::string                             &GetClientInfo(int client_fd) const;

  private:
	VirtualServerStorage virtual_servers_;
	SockContext          sock_context_;
};

} // namespace server

#endif /* CONTEXT_MANAGER_HPP_ */