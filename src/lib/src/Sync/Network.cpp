#include "libtimeit/sync/Network.h"
#include "HTTPRequest.h"
#include <stdexcept>
#include <string.h>
#include <curl/curl.h>

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




HTTPResponse Network::request(const std::string& url, std::string data, std::string username, std::string password,
						  bool ignoreCertificateErrors)
{
	auto request=std::make_shared<HTTPRequest>(ignoreCertificateErrors);
	auto result = request->PUT(url, data, username, password);


	return result;
}

//LCOV_EXCL_STOP
