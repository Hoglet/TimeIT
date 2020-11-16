//
// Created by hoglet on 16/11/2020.
//
#ifndef TIMEIT_HTTPRESPONSE_H
#define TIMEIT_HTTPRESPONSE_H

#include <string>

class HTTPRequest;


class HTTPResponse
{
	friend class HTTPRequest;
public:
	std::string url();
	std::string response();
	bool        statusOK();
	int         httpCode();
	std::string errorMessage();
	HTTPResponse(
			std::string url,
			std::string response,
			bool statusOK,
			int httpCode,
			std::string errorMessage
	);
private:
	HTTPResponse();
	std::string url_;
	std::string response_;
	bool        statusOK_;
	int         httpCode_;
	std::string errorMessage_;
};

#endif //TIMEIT_HTTPRESPONSE_H
