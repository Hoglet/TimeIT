#ifndef TIMEIT_NETWORK_H
#define TIMEIT_NETWORK_H

#include <curl/curl.h>
#include <string>
#include <memory>
#include <future>
#include "http_response.h"
#include "http_request.h"

//LCOV_EXCL_START
namespace libtimeit
{
using namespace std;

struct async_http_response
{
	shared_future<http_response> future_response;
};

class abstract_network
{
public:
	virtual shared_ptr<async_http_response>
	request(
			string url,
			string data,
			string username,
			string password,
			bool   ignore_certificate_errors) = 0;
};


class curl_network : public abstract_network
{
public:
	curl_network();
	curl_network(const curl_network& ) = delete;
	curl_network(curl_network&& )      = delete;
	curl_network& operator=(const curl_network& ) = delete;
	curl_network& operator=(curl_network&& ) = delete;

	virtual ~curl_network();
	shared_ptr<async_http_response> request(
			string url,
			string data,
			string username,
			string password,
			bool ignore_certificate_errors) override;
private:
};
//LCOV_EXCL_STOP

}
#endif