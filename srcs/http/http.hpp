#ifndef HTTP_HPP_
#define HTTP_HPP_

#include "debug.hpp" // todo: tmp
#include <map>
#include <string>

class Http {
  public:
	enum MessageType {
		HTTP_METHOD,
		HTTP_PATH,
		HTTP_VERSION,
		HTTP_CONTENT,
		HTTP_STATUS
	};
	typedef std::map<MessageType, std::string> RequestMessage;
	Http(const std::string &read_buf);
	~Http();
	std::string CreateResponse();

  private:
	Http();
	// prohibit copy
	Http(const Http &other);
	Http &operator=(const Http &other);
	void  ParseRequest(const std::string &read_buf);
	void  ReadPathContent();
	// variable
	RequestMessage request_;
};

#endif /* HTTP_HPP_ */
