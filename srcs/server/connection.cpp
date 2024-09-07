#include "connection.hpp"
#include "client_info.hpp"
#include "server.hpp"
#include "server_info.hpp"
#include "utils.hpp"    // ConvertUintToStr
#include <netdb.h>      // getaddrinfo,freeaddrinfo
#include <netinet/in.h> // struct sockaddr
#include <stdexcept>    // runtime_error
#include <sys/socket.h> // socket,setsockopt,bind,listen,accept
#include <unistd.h>     // close

namespace server {

Connection::Connection() {}

Connection::~Connection() {}

namespace {

void InitHints(Connection::AddrInfo *hints) {
	hints->ai_socktype = SOCK_STREAM;
	hints->ai_family   = AF_UNSPEC;
	hints->ai_flags    = AI_PASSIVE | AI_NUMERICSERV;
}

// 32bit(4bytes) -> xxx:xxx:xxx:xxxx
// struct in_addr addr = {0xc0, 0xa8, 0x01, 0x01}
// -> 192.168.1.1
std::string ConvertToIpv4Str(const struct in_addr &addr) {
	const uint32_t ip_addr = ntohl(addr.s_addr);

	std::ostringstream oss;
	for (int i = 24; i >= 0; i -= 8) {
		oss << ((ip_addr >> i) & 0xff);
		if (i != 0) {
			oss << '.';
		}
	}
	return oss.str();
}

// 128bit(16bytes) -> xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
// struct in6_addr addr = {0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
//                         0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34}
// -> 2001:db8:85a3:0:0:8a2e:370:7334
std::string ConvertToIpv6Str(const struct in6_addr &addr) {
	const uint8_t *bytes = addr.s6_addr;

	std::ostringstream oss;
	oss << std::hex;
	for (unsigned int i = 0; i < 16; i += 2) {
		if (i != 0) {
			oss << ':';
		}
		oss << ((bytes[i] << 8) | bytes[i + 1]);
	}
	oss << std::dec;
	return oss.str();
}

} // namespace

// result: dynamic allocated by getaddrinfo()
Connection::AddrInfo *Connection::GetAddrInfoList(const ServerInfo &server_info) {
	const std::string &port  = utils::ConvertUintToStr(server_info.GetPort());
	AddrInfo           hints = {};
	InitHints(&hints);

	AddrInfo *result;
	const int status = getaddrinfo(server_info.GetHost().c_str(), port.c_str(), &hints, &result);
	// EAI_MEMORY is also included in status != 0
	if (status != 0) {
		throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(status)));
	}
	return result;
}

Connection::BindResult Connection::TryBind(AddrInfo *addrinfo) const {
	BindResult bind_result(false, -1);

	for (; addrinfo != NULL; addrinfo = addrinfo->ai_next) {
		// socket
		const int server_fd =
			socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
		if (server_fd == SYSTEM_ERROR) {
			continue;
		}
		// set socket option to reuse address
		// optval : Specify a non-zero value to enable the boolean option, or zero to disable it
		int optval = 1;
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
			SYSTEM_ERROR) {
			close(server_fd);
			continue;
		}

		// bind
		if (bind(server_fd, addrinfo->ai_addr, addrinfo->ai_addrlen) == SYSTEM_ERROR) {
			close(server_fd);
			continue;
		}
		// bind success
		bind_result.Set(true, server_fd);
		break;
	}
	return bind_result;
}

int Connection::Connect(ServerInfo &server_info) {
	AddrInfo        *addrinfo_list = GetAddrInfoList(server_info);
	const BindResult bind_result   = TryBind(addrinfo_list);
	freeaddrinfo(addrinfo_list);
	if (!bind_result.IsOk()) {
		throw std::runtime_error("bind failed");
	}
	const int server_fd = bind_result.GetValue();

	// todo / listen() : set an appropriate backlog value
	// listen
	if (listen(server_fd, 3) == SYSTEM_ERROR) {
		close(server_fd);
		throw std::runtime_error("listen failed");
	}
	listen_server_fds_.insert(server_fd);

	return server_fd;
}

Connection::IpPortPair Connection::GetListenIpPort(int client_fd) {
	struct sockaddr_storage listen_sock_addr     = {};
	socklen_t               listen_sock_addr_len = sizeof(listen_sock_addr);
	if (getsockname(client_fd, (struct sockaddr *)&listen_sock_addr, &listen_sock_addr_len) ==
		SYSTEM_ERROR) {
		throw std::runtime_error("getsockname failed");
	}

	std::string  listen_ip;
	unsigned int listen_port = 0;
	if (listen_sock_addr.ss_family == AF_INET) {
		struct sockaddr_in *sa_in = (struct sockaddr_in *)&listen_sock_addr;
		listen_ip                 = ConvertToIpv4Str(sa_in->sin_addr);
		listen_port               = ntohs(sa_in->sin_port);
	} else if (listen_sock_addr.ss_family == AF_INET6) {
		struct sockaddr_in6 *sa_in6 = (struct sockaddr_in6 *)&listen_sock_addr;
		listen_ip                   = ConvertToIpv6Str(sa_in6->sin6_addr);
		listen_port                 = ntohs(sa_in6->sin6_port);
	}
	return std::make_pair(listen_ip, listen_port);
}

// todo: return ClientInfo *?
ClientInfo Connection::Accept(int server_fd) {
	struct sockaddr_storage client_sock_addr = {};
	socklen_t               addrlen          = sizeof(client_sock_addr);
	const int client_fd = accept(server_fd, (struct sockaddr *)&client_sock_addr, &addrlen);
	if (client_fd == SYSTEM_ERROR) {
		throw std::runtime_error("accept failed");
	}

	const IpPortPair   listen_ip_port = GetListenIpPort(client_fd);
	const std::string &listen_ip      = listen_ip_port.first;
	const unsigned int listen_port    = listen_ip_port.second;

	// create new client struct
	ClientInfo client_info(client_fd, listen_ip, listen_port);
	utils::Debug("server", "new ClientInfo created. fd", client_fd);
	return client_info;
}

bool Connection::IsListenServerFd(int sock_fd) const {
	return listen_server_fds_.count(sock_fd) == 1;
}

} // namespace server
