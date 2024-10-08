#include "read.hpp"
#include <cerrno>
#include <cstring>  // strerror
#include <unistd.h> // read

namespace server {

Read::Read() {}

Read::~Read() {}

Read::ReadResult Read::ReadStr(int client_fd) {
	ReadResult read_result;

	char    buffer[BUFFER_SIZE];
	ssize_t read_ret = read(client_fd, buffer, BUFFER_SIZE);
	if (read_ret <= 0) {
		if (read_ret == SYSTEM_ERROR) {
			utils::PrintError(__func__, strerror(errno));
			read_result.Set(false);
		}
		const ReadBuf read_buf = {read_ret, ""};
		read_result.SetValue(read_buf);
		return read_result;
	}
	const ReadBuf read_buf = {read_ret, std::string(buffer, read_ret)};
	read_result.SetValue(read_buf);
	return read_result;
}

} // namespace server
