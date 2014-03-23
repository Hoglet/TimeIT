#ifndef NETWORK_H_
#define NETWORK_H_

#include <curl/curl.h>
#include <string>
//LCOV_EXCL_START

class INetwork
{
public:
	virtual ~INetwork();
	virtual std::string request(const std::string& url, std::string data, std::string username, std::string password) = 0;
};


class Network: public INetwork
{
public:
	Network();
	virtual ~Network();
	virtual std::string request(const std::string& url, std::string data, std::string username, std::string password);
	void receiveData(void *ptr, size_t size, size_t nmemb);
	size_t sendData(void *ptr, size_t size, size_t nmemb);
private:
	CURL *init();
	void close(CURL* curl);
	std::string receivedData;
	std::string dataToSend;
	int curSendPosition;
};
//LCOV_EXCL_STOP

#endif /* NETWORK_H_ */
