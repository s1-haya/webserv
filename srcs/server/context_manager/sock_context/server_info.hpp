#ifndef SOCK_INFO_HPP_
#define SOCK_INFO_HPP_

#include <string>

namespace server {

class ServerInfo {
  public:
	// default constructor: necessary for map's insert/[]
	ServerInfo();
	explicit ServerInfo(unsigned int port);
	~ServerInfo();
	ServerInfo(const ServerInfo &other);
	ServerInfo &operator=(const ServerInfo &other);
	// getter
	int          GetFd() const;
	unsigned int GetPort() const;
	// setter
	void SetSockFd(int fd);

  private:
	int          fd_;
	unsigned int port_;
};

} // namespace server

#endif /* SOCK_INFO_HPP_ */