#ifndef TMP_HTTP_HPP_
#define TMP_HTTP_HPP_

#include "http_parse.hpp"
#include "http_storage.hpp"

namespace http {

class TmpHttp {
	public:
	TmpHttp();
	~TmpHttp();

	private:
		TmpHttp(const TmpHttp &other);
		TmpHttp           &operator=(const TmpHttp &other);
		static HttpStorage storage_;
		// todo: parse class
		static HttpParse parse_;
		// todo: response class
};

} // namespace http

#endif
