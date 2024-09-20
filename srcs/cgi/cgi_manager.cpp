#include "cgi_manager.hpp"
#include "cgi.hpp"

namespace cgi {

CgiManager::CgiManager() {}

CgiManager::~CgiManager() {
	for (ItrCgiAddrMap it = client_cgi_map_.begin(); it != client_cgi_map_.end(); ++it) {
		delete it->second;
	}
}

void CgiManager::AddNewCgi(int client_fd, const CgiRequest &request) {
	// todo: iru?
	// if (cgi_map_.count(client_fd) != 0) {
	// 	return;
	// }
	Cgi *cgi = new Cgi(request);
	// todo: new error handling
	client_cgi_map_[client_fd] = cgi;
}

Cgi *CgiManager::GetCgi(int client_fd) const {
	// todo: logic_error?
	return client_cgi_map_.at(client_fd);
}

bool CgiManager::IsClientFd(int fd) const {
	return client_cgi_map_.count(fd) == 1;
}

} // namespace cgi
