#ifndef TIMEIT_NETWORK_H
#define TIMEIT_NETWORK_H

#include <curl/curl.h>
#include <string>
#include <memory>
#include <future>
#include "HTTP_response.h"
#include "HTTP_request.h"

//LCOV_EXCL_START
namespace libtimeit
{
using namespace std;

struct asyncHTTPResponse
{
	shared_future<HTTP_response> futureResponse;
};

class INetwork
{
public:
	virtual shared_ptr<asyncHTTPResponse>
	request(
			string url,
			string data,
			string username,
			string password,
			bool   ignore_certificate_errors) = 0;
};


class Network : public INetwork
{
public:
	Network();
	Network( const Network& ) = delete;
	Network( Network&& )      = delete;
	Network& operator=( const Network& ) = delete;
	Network& operator=(  Network&& ) = delete;

	virtual ~Network();
	shared_ptr<asyncHTTPResponse> request(
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