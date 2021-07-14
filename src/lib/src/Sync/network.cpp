#include "libtimeit/sync/network.h"
#include "libtimeit/sync/http_request.h"
#include <stdexcept>
#include <curl/curl.h>
#include <future>

//LCOV_EXCL_START
namespace libtimeit
{


using namespace std;

curl_network::curl_network()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

curl_network::~curl_network()
{
	curl_global_cleanup();
}


shared_ptr<async_http_response> curl_network::request(
		string url,
		string data,
		string username,
		string password,
		bool ignore_certificate_errors)
{


	shared_ptr<async_http_response> result = make_shared<async_http_response>();


	result->future_response = async(
			launch::async,
			[url, data, username, password, ignore_certificate_errors]() {
				http_request request;
				request.ignore_cert_errors(ignore_certificate_errors);
				return request.put(url, data, username, password);
			}
	).share();

	return result;
}

}
//LCOV_EXCL_STOP
