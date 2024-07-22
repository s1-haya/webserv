#include "client_info.hpp"
#include "color.hpp"
#include "server_info.hpp"
#include "sock_context.hpp"
#include "utils.hpp"
#include <cstdlib>
#include <iostream>
#include <string>

namespace {

int GetTestCaseNum() {
	static unsigned int test_case_num = 0;
	++test_case_num;
	return test_case_num;
}

void PrintOk() {
	std::cout << utils::color::GREEN << GetTestCaseNum() << ".[OK] " << utils::color::RESET
			  << std::endl;
}

void PrintNg() {
	std::cerr << utils::color::RED << GetTestCaseNum() << ".[NG] " << utils::color::RESET
			  << std::endl;
}

void PrintError(const std::string &message) {
	std::cerr << utils::color::RED << message << utils::color::RESET << std::endl;
}

// -----------------------------------------------------------------------------
bool IsSameClientInfo(const server::ClientInfo &a, const server::ClientInfo &b) {
	return a.GetFd() == b.GetFd() && a.GetIp() == b.GetIp();
}

// ClientInfo同士のメンバが全て等しいことを期待するテスト
// (todo: 呼び出し側でClientInfoのメンバがセットできてないので今必ずtrueが返る)
int TestIsSameClientInfo(
	const server::SockContext                &context,
	const server::SockContext::ClientInfoMap &expected_client_info,
	int                                       client_fd
) {
	// テスト対象のgetter
	const server::ClientInfo &a = context.GetClientInfo(client_fd);
	const server::ClientInfo &b = expected_client_info.at(client_fd);

	if (IsSameClientInfo(a, b)) {
		PrintOk();
		return EXIT_SUCCESS;
	}
	PrintNg();
	std::cerr << "client_fd : result   [" << a.GetFd() << "]" << std::endl;
	std::cerr << "            expected [" << b.GetFd() << "]" << std::endl;
	std::cerr << "client_IP : result   [" << a.GetIp() << "]" << std::endl;
	std::cerr << "            expected [" << b.GetIp() << "]" << std::endl;
	return EXIT_FAILURE;
}

bool IsSameServerInfo(const server::ServerInfo &a, const server::ServerInfo &b) {
	return a.GetFd() == b.GetFd() && a.GetName() == b.GetName() && a.GetPort() == b.GetPort();
}

// ServerInfo同士のメンバが全て等しいことを期待するテスト
int TestIsSameHostServerInfo(
	const server::SockContext                    &context,
	const server::SockContext::HostServerInfoMap &expected_host_server_info,
	int                                           client_fd
) {
	// テスト対象のgetter
	const server::ServerInfo &a = context.GetConnectedServerInfo(client_fd);
	const server::ServerInfo &b = *expected_host_server_info.at(client_fd);

	if (IsSameServerInfo(a, b)) {
		PrintOk();
		return EXIT_SUCCESS;
	}
	PrintNg();
	std::cerr << "server_fd  : result   [" << a.GetFd() << "]" << std::endl;
	std::cerr << "             expected [" << b.GetFd() << "]" << std::endl;
	std::cerr << "server_name: result   [" << a.GetName() << "]" << std::endl;
	std::cerr << "             expected [" << b.GetName() << "]" << std::endl;
	std::cerr << "port       : result   [" << a.GetPort() << "]" << std::endl;
	std::cerr << "             expected [" << b.GetPort() << "]" << std::endl;
	return EXIT_FAILURE;
}

// test_funcを実行したらthrowされることを期待するテスト
template <typename TestFunc>
int TestThrow(
	TestFunc                  test_func,
	server::SockContext      &context,
	int                       server_fd,
	const server::ServerInfo &server_info
) {
	try {
		(context.*test_func)(server_fd, server_info);
		PrintNg();
		PrintError("throw failed");
		return EXIT_FAILURE;
	} catch (const std::exception &e) {
		PrintOk();
		std::cerr << utils::color::GRAY << e.what() << utils::color::RESET << std::endl;
		return EXIT_SUCCESS;
	}
}

// -----------------------------------------------------------------------------
/*

以下のような接続状況を仮定しテストする

fd | listen_server |  client
--------------------------------
 4 |  ServerInfo1  |
 5 |  ServerInfo2  |
 6 |  ServerInfo1  | ClientInfo1 (accept済み接続)
 7 |  ServerInfo2  | ClientInfo2 (accept済み接続)

SockContext内部の期待されるデータ保持
- ServerInfoMap     = {{4, ServerInfo1}, {5, ServerInfo2}}
- ClientInfoMap     = {{6, ClientInfo1}, {7, ClientInfo2}}
- HostServerInfoMap = {{6, ServerInfo1*}, {7, ServerInfo2*}}

*/
int RunTestSockContext() {
	int ret_code = EXIT_SUCCESS;

	/* ----------- 準備 ----------- */
	// SockContextに渡す用のServerInfoを2個作成
	server::ServerInfo server_info1("localhost", 8080);
	server_info1.SetSockFd(4);
	server::ServerInfo server_info2("abcde", 12345);
	server_info2.SetSockFd(5);

	// SockContextに渡す用のClientInfoを2個作成
	// (todo:ClientInfoがsock_addrをコンストラクト時に渡さないといけないことによりfd,portがセットできてない)
	server::ClientInfo client_info1;
	server::ClientInfo client_info2;

	// 期待するSockContextのメンバのmap(同時に自分で作成していく)
	server::SockContext::ServerInfoMap     expected_server_info;
	server::SockContext::ClientInfoMap     expected_client_info;
	server::SockContext::HostServerInfoMap expected_host_server_info;

	/* ----------- テスト対象 : class SockContextのpublicメンバ関数 ----------- */
	server::SockContext context;

	// contextにServerInfo1を追加
	// - ServerInfoMap      = {{4, ServerInfo1}}
	// - ClientInfoMap      = {}
	// - HostServerInfopMap = {}
	const int server_fd1 = 4;
	context.AddServerInfo(server_fd1, server_info1);
	expected_server_info[server_fd1] = server_info1;

	// contextにServerInfo2を追加
	// - ServerInfoMap      = {{4, ServerInfo1}, {5, ServerInfo2}}
	// - ClientInfoMap      = {}
	// - HostServerInfopMap = {}
	const int server_fd2 = 5;
	context.AddServerInfo(server_fd2, server_info2);
	expected_server_info[server_fd2] = server_info2;

	// contextに既に追加済みのServerInfo2を再度追加してみる(期待: throw)
	ret_code |= TestThrow(&server::SockContext::AddServerInfo, context, server_fd2, server_info2);

	// contextにClientInfo1とそれに紐づくserver_fd1を追加
	// - ServerInfoMap     = {{4, ServerInfo1}, {5, ServerInfo2}}
	// - ClientInfoMap     = {{6, ClientInfo1}}
	// - HostServerInfoMap = {{6, ServerInfo1*}}
	const int client_fd1 = 6;
	context.AddClientInfo(client_fd1, client_info1, server_fd1);
	expected_client_info[client_fd1]      = client_info1;
	expected_host_server_info[client_fd1] = &server_info1;
	// contextのメンバと自作のexpectedが同じか確認
	ret_code |= TestIsSameClientInfo(context, expected_client_info, client_fd1);
	ret_code |= TestIsSameHostServerInfo(context, expected_host_server_info, client_fd1);

	// contextにClientInfo2とServerInfoMapに登録されていないserver_fdを追加してみる (todo: 保留)
	// ret_code |= TestThrow(&server::SockContext::AddClientInfo, client_fd2, client_info2, 100);

	// contextにClientInfo2とそれに紐づくserver_fd2を追加
	// - ServerInfoMap     = {{4, ServerInfo1}, {5, ServerInfo2}}
	// - ClientInfoMap     = {{6, ClientInfo1}, {7, ClientInfo2}}
	// - HostServerInfoMap = {{6, ServerInfo1*}, {7, ServerInfo2*}}
	const int client_fd2 = 7;
	context.AddClientInfo(client_fd2, client_info2, server_fd2);
	expected_client_info[client_fd2]      = client_info2;
	expected_host_server_info[client_fd2] = &server_info2;
	// contextのメンバと自作のexpectedが同じか確認
	ret_code |= TestIsSameClientInfo(context, expected_client_info, client_fd2);
	ret_code |= TestIsSameHostServerInfo(context, expected_host_server_info, client_fd2);

	return ret_code;
}

} // namespace

int main() {
	int ret_code = EXIT_SUCCESS;

	ret_code |= RunTestSockContext();

	return ret_code;
}
