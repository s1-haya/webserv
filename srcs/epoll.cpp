#include "epoll.hpp"
#include "utils.hpp"
#include <errno.h>
#include <stdint.h> // uint32_t
#include <unistd.h> // close

namespace epoll {

Epoll::Epoll() {
	epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd_ == SYSTEM_ERROR) {
		throw std::runtime_error("epoll_create failed");
	}
}

Epoll::~Epoll() {
	if (epoll_fd_ != SYSTEM_ERROR) {
		close(epoll_fd_);
	}
}

namespace {

uint32_t ConvertToEpollEventType(event::Type type) {
	uint32_t ret_type = 0;

	if (type & event::EVENT_READ) {
		ret_type |= EPOLLIN;
	}
	if (type & event::EVENT_WRITE) {
		ret_type |= EPOLLOUT;
	}
	return ret_type;
}

uint32_t ConvertToEventType(uint32_t event) {
	uint32_t ret_type = event::EVENT_NONE;

	if (event & EPOLLIN) {
		ret_type |= event::EVENT_READ;
	}
	if (event & EPOLLOUT) {
		ret_type |= event::EVENT_WRITE;
	}
	// todo: tmp
	return ret_type;
}

event::Event ConvertToEventDto(const struct epoll_event &event) {
	event::Event ret_event;
	ret_event.type      = ConvertToEventType(event.events);
	ret_event.sock_info = static_cast<server::SockInfo *>(event.data.ptr);
	return ret_event;
}

} // namespace

void Epoll::AddNewConnection(server::SockInfo *sock_info, event::Type type) {
	struct epoll_event ev;
	ev.events   = ConvertToEpollEventType(type);
	ev.data.ptr = sock_info;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, sock_info->fd, &ev) == SYSTEM_ERROR) {
		throw std::runtime_error("epoll_ctl failed");
	}
}

// todo: error?
void Epoll::DeleteConnection(server::SockInfo *sock_info) {
	const int client_fd = sock_info->fd;
	delete sock_info;
	epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client_fd, NULL);
}

int Epoll::CreateReadyList() {
	errno           = 0;
	const int ready = epoll_wait(epoll_fd_, evlist_, MAX_EVENTS, -1);
	utils::Debug("epoll", "ready list", ready);
	if (ready == SYSTEM_ERROR) {
		if (errno == EINTR) {
			return ready;
		}
		throw std::runtime_error("epoll_wait failed");
	}
	return ready;
}

// override with new_type
void Epoll::UpdateEventType(
	int socket_fd, server::SockInfo *sock_info, const event::Type new_type
) {
	struct epoll_event ev;
	ev.events   = ConvertToEpollEventType(new_type);
	ev.data.ptr = sock_info;
	if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, socket_fd, &ev) == SYSTEM_ERROR) {
		throw std::runtime_error("epoll_ctl failed");
	}
}

event::Event Epoll::GetEvent(std::size_t index) const {
	if (index >= MAX_EVENTS) {
		throw std::out_of_range("evlist index out of range");
	}
	return ConvertToEventDto(evlist_[index]);
}

} // namespace epoll
