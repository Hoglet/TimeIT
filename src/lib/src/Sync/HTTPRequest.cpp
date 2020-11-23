//
// Created by hoglet on 14/11/2020.
//

#include "libtimeit/sync/HTTPRequest.h"
#include <cstring>
#include <stdexcept>

size_t receive_data(void *ptr, size_t size, size_t nmemb, std::string *receivedData)
{
	int length = size * nmemb;
	char data[length + 1];
	strncpy(data, (const char*) ptr, length);
	data[length] = 0;
	receivedData->append(data);
	return size * nmemb;
}

size_t send_data(void *ptr, size_t size, size_t nmemb, HTTPRequest *caller)
{
	return caller->sendData(ptr, size, nmemb);
}

HTTPRequest::HTTPRequest()
{
	curl = curl_easy_init();

	if (curl == nullptr)
	{
		throw std::runtime_error("Unable to initialise network");
	}
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_data);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, send_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receiveBuffer);
	curl_easy_setopt(curl, CURLOPT_READDATA, this);


}

void HTTPRequest::ignoreCertErrors(bool ignore)
{
	ignoreCertificateErrors_ = ignore;
}



size_t HTTPRequest::sendData(void *ptr, size_t size, size_t nmemb)
{
	size_t dataLength = sendBuffer.size();
	size_t bufferLength = size * nmemb;
	size_t charactersToSend = dataLength - curSendPosition;
	if (charactersToSend > bufferLength)
	{
		charactersToSend = bufferLength;
	}

	strncpy((char*) ptr, &(sendBuffer.c_str()[curSendPosition]), charactersToSend);
	curSendPosition += charactersToSend;
	return charactersToSend;

}

HTTPRequest::~HTTPRequest()
{
	curl_easy_cleanup(curl);
};

HTTPResponse HTTPRequest::PUT(
		const std::string& url,
		std::string data,
		std::string username,
		std::string password
)
{
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	receiveBuffer.clear();
	sendBuffer = data;
	curSendPosition = 0;

	struct curl_slist *headers=NULL; // init to NULL is important
	headers=curl_slist_append(headers, "Accept: application/json");
	headers=curl_slist_append( headers, "Content-Type: application/json");
	headers=curl_slist_append( headers, "charsets: utf-8");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_BASIC);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	if (ignoreCertificateErrors_)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}
	/* Perform the request, res will get the return code */
	CURLcode res = curl_easy_perform(curl);

	int httpCode;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

	HTTPResponse result(
			url,
			receiveBuffer,
			(res == CURLE_OK),
			httpCode,
			curl_easy_strerror(res));

	curl_slist_free_all(headers);
	return result;
}