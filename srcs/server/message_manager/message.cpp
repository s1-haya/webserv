#include "message.hpp"

namespace server {
namespace message {

Message::Message(int client_fd) : client_fd_(client_fd), start_time_(GetCurrentTime()) {}

Message::~Message() {}

Message::Message(const Message &other) {
	*this = other;
}

Message &Message::operator=(const Message &other) {
	if (this != &other) {
		client_fd_   = other.client_fd_;
		start_time_  = other.start_time_;
		request_buf_ = other.request_buf_;
		response_    = other.response_;
	}
	return *this;
}

bool Message::IsTimeoutExceeded(double timeout_sec) const {
	const Time   current_time  = GetCurrentTime();
	const double diff_time_sec = std::difftime(current_time, start_time_);
	return diff_time_sec >= timeout_sec;
}

int Message::GetFd() const {
	return client_fd_;
}

const std::string &Message::GetRequestBuf() const {
	return request_buf_;
}

const std::string &Message::GetResponse() const {
	return response_;
}

// todo: Httpと繋がったら += ではなく = になる
void Message::SetRequestBuf(const std::string &request_buf) {
	request_buf_ += request_buf;
}

void Message::SetResponse(const std::string &response) {
	response_ = response;
}

Message::Time Message::GetCurrentTime() {
	return std::time(NULL);
}

} // namespace message
} // namespace server
