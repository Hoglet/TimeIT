#include "libtimeit/sync/Network.h"
#include "libtimeit/sync/HTTPRequest.h"
#include <stdexcept>
#include <string.h>
#include <curl/curl.h>
#include <future>

//LCOV_EXCL_START

INetwork::~INetwork()
= default;


Network::Network()
{
	curl_global_init(CURL_GLOBAL_ALL);

}

Network::~Network()
{
	curl_global_cleanup();
}




std::shared_ptr<asyncHTTPResponse> Network::request(const std::string& url, std::string data, std::string username, std::string password,
													bool ignoreCertificateErrors)
{
	std::shared_ptr<asyncHTTPResponse> result=std::make_shared<asyncHTTPResponse>();

	result->request.ignoreCertErrors(ignoreCertificateErrors);

	result->futureResponse =	std::async(
					std::launch::async,
					&HTTPRequest::PUT,
					result->request,
					url, data, username, password).share();

	return result;
}

//LCOV_EXCL_STOP
