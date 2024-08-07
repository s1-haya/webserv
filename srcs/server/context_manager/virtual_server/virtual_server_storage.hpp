#ifndef SERVER_CONTEXTMANAGER_VIRTUALSERVER_VIRTUALSERVERSTORAGE_HPP_
#define SERVER_CONTEXTMANAGER_VIRTUALSERVER_VIRTUALSERVERSTORAGE_HPP_

#include <list>
#include <map>

namespace server {

class VirtualServer;

// fdとVirtualServerを紐づけて全VirtualServerを保持・取得する
class VirtualServerStorage {
  public:
	typedef std::list<VirtualServer>             VirtualServerList;
	typedef std::map<int, const VirtualServer *> VirtualServerMapping;
	VirtualServerStorage();
	~VirtualServerStorage();
	VirtualServerStorage(const VirtualServerStorage &other);
	VirtualServerStorage &operator=(const VirtualServerStorage &other);
	// functions
	void AddVirtualServer(const VirtualServer &virtual_server);
	void AddMapping(int server_fd, const VirtualServer *virtual_server);
	// getter
	const VirtualServer     &GetVirtualServer(int server_fd) const;
	const VirtualServerList &GetAllVirtualServerList() const;

  private:
	// 全VirtualServerを保持
	VirtualServerList virtual_servers_;
	// VirtualServerと、VirtualServerが通信に使用している複数fdを紐づける
	VirtualServerMapping mapping_fd_to_virtual_servers_;
};

} // namespace server

#endif /* SERVER_CONTEXTMANAGER_VIRTUALSERVER_VIRTUALSERVERSTORAGE_HPP_ */
