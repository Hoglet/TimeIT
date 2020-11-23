//
// Created by hoglet on 14/11/2020.
//
#ifndef TIMEIT_HTTPREQUEST_H
#define TIMEIT_HTTPREQUEST_H

#include <libtimeit/sync/HTTPResponse.h>
#include <curl/curl.h>
#include <string>




class HTTPRequest
{
	friend size_t send_data(void*, size_t, size_t, HTTPRequest*);
public:
	HTTPRequest();
	~HTTPRequest();
	void ignoreCertErrors( bool );
	HTTPResponse PUT(
			const std::string& url,
			std::string data,
			std::string username,
			std::string password
			);
private:
	size_t sendData(void *pVoid, size_t i, size_t i1);

	CURL*       curl;
	std::string receiveBuffer;
	std::string sendBuffer;

	size_t curSendPosition{0};
	bool ignoreCertificateErrors_{false};
};


#endif //TIMEIT_HTTPREQUEST_H
