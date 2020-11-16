#pragma once

#include <curl/curl.h>
#include <string>
#include <memory>
#include "HTTPResponse.h"
//LCOV_EXCL_START


class INetwork
{
public:
	virtual ~INetwork();
	virtual HTTPResponse request(const std::string& url, std::string data, std::string username, std::string password, bool ignoreCertificateErrors) = 0;
};


class Network: public INetwork
{
public:
	Network();
	virtual ~Network();
	virtual HTTPResponse request(const std::string& url, std::string data, std::string username, std::string password, bool ignoreCertificateErrors);
private:
};
//LCOV_EXCL_STOP


