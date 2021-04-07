#ifndef TIMEIT_NETWORK_H
#define TIMEIT_NETWORK_H

#include <curl/curl.h>
#include <string>
#include <memory>
#include <future>
#include "HTTPResponse.h"
#include "HTTPRequest.h"

//LCOV_EXCL_START
namespace libtimeit
{
using namespace std;

struct asyncHTTPResponse
{
	shared_future<HTTPResponse> futureResponse;
};

class INetwork
{
public:
	virtual ~INetwork();
	virtual shared_ptr<asyncHTTPResponse>
	request(
			string url,
			string data,
			string username,
			string password,
			bool   ignoreCertificateErrors) = 0;
};


class Network : public INetwork
{
public:
	Network();
	virtual ~Network();
	virtual std::shared_ptr<asyncHTTPResponse> request(
			string url,
			string data,
			string username,
			string password,
			bool ignoreCertificateErrors);
private:
};
//LCOV_EXCL_STOP

}
#endif