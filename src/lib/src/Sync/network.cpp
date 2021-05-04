#include "libtimeit/sync/network.h"
#include "libtimeit/sync/HTTP_request.h"
#include <stdexcept>
#include <curl/curl.h>
#include <future>

//LCOV_EXCL_START
namespace libtimeit
{


using namespace std;

Network::Network()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

Network::~Network()
{
	curl_global_cleanup();
}


shared_ptr<asyncHTTPResponse> Network::request(
		string url,
		string data,
		string username,
		string password,
		bool ignore_certificate_errors)
{


	shared_ptr<asyncHTTPResponse> result = make_shared<asyncHTTPResponse>();


	result->futureResponse = async(
			launch::async,
			[url, data, username, password, ignore_certificate_errors]() {
				HTTP_request request;
				request.ignore_cert_errors(ignore_certificate_errors);
				return request.PUT(url, data, username, password);
			}
	).share();

	return result;
}

}
//LCOV_EXCL_STOP
