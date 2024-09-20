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

void CgiManager::DeleteCgi(int client_fd) {
	const Cgi *cgi = GetCgi(client_fd);

	// FdMapから削除
	if (cgi->IsReadRequired()) {
		pipe_fd_map_.erase(cgi->GetReadFd());
	}
	if (cgi->IsWriteRequired()) {
		pipe_fd_map_.erase(cgi->GetWriteFd());
	}
	// cgi*削除
	delete cgi;
	// CgiAddrMap削除
	client_cgi_map_.erase(client_fd);
}

void CgiManager::RunCgi(int client_fd) {
	Cgi *cgi = GetCgi(client_fd);

	const Cgi::CgiResult cgi_result = cgi->Run();
	if (!cgi_result.IsOk()) {
		// todo: error handling
		return;
	}
	// pipe_fdとclient_fdの紐づけを保持
	if (cgi->IsReadRequired()) {
		pipe_fd_map_[cgi->GetReadFd()] = client_fd;
	}
	if (cgi->IsWriteRequired()) {
		pipe_fd_map_[cgi->GetWriteFd()] = client_fd;
	}
}

Cgi *CgiManager::GetCgi(int client_fd) const {
	// todo: logic_error?
	return client_cgi_map_.at(client_fd);
}

bool CgiManager::IsClientFd(int fd) const {
	return client_cgi_map_.count(fd) == 1;
}

} // namespace cgi