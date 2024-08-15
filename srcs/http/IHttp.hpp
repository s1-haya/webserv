#ifndef IHTTP_HPP_
#define IHTTP_HPP_

namespace http {

struct HttpResult;

class IHttp {
  public:
	virtual ~IHttp() {}

	// todo: update params
	/**
	 * @brief Processes the HTTP request and generates a response.
	 *
	 * This method handles the incoming HTTP request, performs the necessary
	 * operations, and produces an appropriate response.
	 *
	 * @param client_infos(tmp)
	 * @param server_infos(tmp)
	 *
	 * @return HttpResult indicating whether the response is complete
	 *         and containing the response data.
	 */
	virtual HttpResult Run() = 0;
};

} // namespace http

#endif /* IHTTP_HPP_ */