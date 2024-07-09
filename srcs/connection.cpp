#include "connection.hpp"
#include "server.hpp"
#include "sock_info.hpp"
#include "utils.hpp"    // ConvertUintToStr
#include <netdb.h>      // getaddrinfo,freeaddrinfo
#include <stdexcept>    // runtime_error
#include <sys/socket.h> // socket,setsockopt,bind,listen,accept

namespace server {

Connection::Connection() {}

Connection::~Connection() {}

namespace {

void InitHints(Connection::AddrInfo *hints) {
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_family   = AF_UNSPEC;
	hints->ai_flags    = AI_PASSIVE | AI_NUMERICSERV;
}

} // namespace

// result: dynamic allocated by getaddrinfo()
Connection::AddrInfo *Connection::GetAddrInfoList(const SockInfo &server_sock_info) {
	const std::string &host  = server_sock_info.GetName();
	const std::string &port  = utils::ConvertUintToStr(server_sock_info.GetPort());
	AddrInfo           hints = {};
	InitHints(&hints);

	AddrInfo *result;
	const int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
	if (status != 0) {
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));
	}
	return result;
}

int Connection::Connect(SockInfo &server_sock_info) {
	AddrInfo *addrinfo_list = GetAddrInfoList(server_sock_info);
	(void)addrinfo_list;

	// socket
	const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == SYSTEM_ERROR) {
		throw std::runtime_error("socket failed");
	}
	// set socket option to reuse address
	// optval : Specify a non-zero value to enable the boolean option, or zero to disable it
	int optval = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == SYSTEM_ERROR) {
		throw std::runtime_error("setsockopt failed");
	}

	struct sockaddr_in &sock_addr = server_sock_info.GetSockAddr();
	const socklen_t     addrlen   = server_sock_info.GetAddrlen();
	// bind
	if (bind(server_fd, (const struct sockaddr *)&sock_addr, addrlen) == SYSTEM_ERROR) {
		throw std::runtime_error("bind failed");
	}

	// todo / listen() : set an appropriate backlog value
	// listen
	if (listen(server_fd, 3) == SYSTEM_ERROR) {
		throw std::runtime_error("listen failed");
	}
	listen_server_fds_.insert(server_fd);

	return server_fd;
}

int Connection::Accept(SockInfo &sock_info) {
	const int           server_fd = sock_info.GetFd();
	struct sockaddr_in &sock_addr = sock_info.GetSockAddr();
	socklen_t           addrlen   = sock_info.GetAddrlen();

	const socklen_t new_socket = accept(server_fd, (struct sockaddr *)&sock_addr, &addrlen);
	// retrieve the client's IP address, port, etc.

	// todo: need?
	// if (new_socket == SYSTEM_ERROR) {
	// 	throw std::runtime_error("accept failed");
	// }
	return new_socket;
}

bool Connection::IsListenServerFd(int sock_fd) const {
	return listen_server_fds_.count(sock_fd) == 1;
}

} // namespace server
