#include "cgi_manager.hpp"

namespace server {

CgiManager::CgiManager() {}

CgiManager::~CgiManager() {
	for (ItrCgiAddrMap it = cgi_addr_map_.begin(); it != cgi_addr_map_.end(); ++it) {
		delete it->second;
	}
}

// todo: 1complete_request(1CGI)につき2回以上この関数が呼ばれないかhttpの実装を確認
void CgiManager::AddNewCgi(int client_fd, const cgi::CgiRequest &request) {
	// todo: new error handling
	Cgi *cgi = new Cgi(request);
	// todo: add logic_error?
	cgi_addr_map_[client_fd] = cgi;
}

CgiManager::Cgi::CgiResult CgiManager::RunCgi(int client_fd) {
	Cgi *cgi = GetCgi(client_fd);

	const Cgi::CgiResult &cgi_result = cgi->Run();
	if (!cgi_result.IsOk()) {
		// todo: error handling?
		return cgi_result;
	}
	// pipe_fdとclient_fdの紐づけをFdMapに追加
	// todo: add logic_error?
	if (cgi->IsReadRequired()) {
		client_fd_map_[cgi->GetReadFd()] = client_fd;
	}
	if (cgi->IsWriteRequired()) {
		client_fd_map_[cgi->GetWriteFd()] = client_fd;
	}
	return cgi_result;
}

void CgiManager::DeleteCgi(int client_fd) {
	const Cgi *cgi = GetCgi(client_fd);

	// FdMapから削除
	if (cgi->IsReadRequired()) {
		client_fd_map_.erase(cgi->GetReadFd());
	}
	if (cgi->IsWriteRequired()) {
		client_fd_map_.erase(cgi->GetWriteFd());
	}
	// cgi*削除
	delete cgi;
	// CgiAddrMapから削除
	cgi_addr_map_.erase(client_fd);
}

bool CgiManager::IsResponseComplete(int client_fd) const {
	const Cgi *cgi = cgi_addr_map_.at(client_fd);
	return cgi->IsResponseComplete();
}

void CgiManager::AddReadBuf(int client_fd, const std::string &read_buf) {
	Cgi *cgi = cgi_addr_map_.at(client_fd);
	cgi->AddReadBuf(read_buf);
}

CgiManager::GetFdResult CgiManager::GetReadFd(int client_fd) const {
	GetFdResult result;

	const Cgi *cgi = cgi_addr_map_.at(client_fd);
	if (!cgi->IsReadRequired()) {
		result.Set(false);
		return result;
	}
	result.SetValue(cgi->GetReadFd());
	return result;
}

CgiManager::GetFdResult CgiManager::GetWriteFd(int client_fd) const {
	GetFdResult result;

	const Cgi *cgi = cgi_addr_map_.at(client_fd);
	if (!cgi->IsWriteRequired()) {
		result.Set(false);
		return result;
	}
	result.SetValue(cgi->GetWriteFd());
	return result;
}

int CgiManager::GetClientFd(int pipe_fd) const {
	// todo: add logic_error?
	return client_fd_map_.at(pipe_fd);
}

const std::string &CgiManager::GetRequest(int client_fd) const {
	const Cgi *cgi = cgi_addr_map_.at(client_fd);
	return cgi->GetRequest();
}

// todo: 返り値がcgi::CgiResponseになりそう
const std::string &CgiManager::GetResponse(int client_fd) const {
	const Cgi *cgi = cgi_addr_map_.at(client_fd);
	return cgi->GetResponse();
}

void CgiManager::ReplaceNewRequest(int client_fd, const std::string &new_request_str) {
	Cgi *cgi = cgi_addr_map_.at(client_fd);
	cgi->ReplaceNewRequest(new_request_str);
}

CgiManager::Cgi *CgiManager::GetCgi(int client_fd) const {
	// todo: add logic_error?
	return cgi_addr_map_.at(client_fd);
}

} // namespace server