#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	const char *server_ip = "127.0.0.1"; // ApacheサーバのIPアドレス
	int         port      = 4242;          // Apacheサーバのポート番号

	// ソケットの作成
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return 1;
	}

	// サーバのアドレス情報の設定
	struct sockaddr_in server_addr;
	std::memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(port);
	if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
		std::cerr << "Invalid address / Address not supported" << std::endl;
		close(sockfd);
		return 1;
	}

	// サーバに接続
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		std::cerr << "Connection failed" << std::endl;
		close(sockfd);
		return 1;
	}

	// HTTP GETリクエストの作成
	std::string request_body = "key1=value1&key2=value2";
	std::string http_request = "GET /cgi-bin/first.pl HTTP/1.1\r\n"
							   "Host: a\r\n\r\n";
	// Hostを複数設定した場合　HTTP/1.1 400 Bad Request
	// std::string http_request = "GET / HTTP/1.1\r\n"
	// 						   "Host: a\r\n"
	// 						   "Host: " + std::string(server_ip) + "\r\n"
	// 						   "Connection: close\r\n"
	// 						   "Content-Type: application/x-www-form-urlencoded\r\n"
	// 						   "Content-Length: " +
	// 						   std::to_string(request_body.length()) + "\r\n\r\n" + request_body;

	// Connectionが複数設定した場合は最後に設定した値が適用: HTTP/1.1 200 OK
	// std::string http_request = "GET / HTTP/1.1\r\n"
	// 						   "Host: a\r\n"
	// 						   "Connection: keep-alive\r\n"
	// 						   "Connection: keep-alive\r\n"
	// 						   "Connection: close\r\n"
	// 						   "Content-Type: application/x-www-form-urlencoded\r\n"
	// 						   "Content-Length: " +
	// 						   std::to_string(request_body.length()) + "\r\n\r\n" + request_body;

	// リクエストの送信
	send(sockfd, http_request.c_str(), http_request.size(), 0);

	// レスポンスの受信
	char buffer[4096];
	int  bytes_received;
	while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[bytes_received] = '\0'; // 受信データを文字列として終端
		std::cout << buffer;
	}

	// ソケットを閉じる
	close(sockfd);

	return 0;
}
