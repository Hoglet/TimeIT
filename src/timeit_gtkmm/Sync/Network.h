#ifndef NETWORK_H_
#define NETWORK_H_

#include <curl/curl.h>
#include <string>
#include <memory>
#include <MessageCenter.h>
//LCOV_EXCL_START

struct NetworkResponse
{
	std::string url;
	std::string response;
	bool statusOK;
	int httpCode;
	std::string errorMessage;
};

class INetwork
{
public:
	virtual ~INetwork();
	virtual struct NetworkResponse request(const std::string& url, std::string data, std::string username, std::string password, bool ignoreCertificateErrors) = 0;
};


class Network: public INetwork
{
public:
	Network();
	virtual ~Network();
	virtual struct NetworkResponse request(const std::string& url, std::string data, std::string username, std::string password, bool ignoreCertificateErrors);
	void receiveData(void *ptr, size_t size, size_t nmemb);
	size_t sendData(void *ptr, size_t size, size_t nmemb);
private:
	CURL *init(bool verifyConnection);
	void close(CURL* curl);
	std::string receivedData;
	std::string dataToSend;
	int curSendPosition;
};
//LCOV_EXCL_STOP

#endif /* NETWORK_H_ */
