#include "MockNetwork.h"

using namespace std;

namespace test
{

MockNetwork::MockNetwork()
{
	// TODO Auto-generated constructor stub

}

MockNetwork::~MockNetwork()
{
	// TODO Auto-generated destructor stub
}

void MockNetwork::setResponse(std::string& uri, std::string& response)
{
	responses[uri] = response;
}

std::shared_ptr<asyncHTTPResponse> MockNetwork::request(const std::string& url, std::string data, std::string username, std::string password,
		bool verifyPassword)
{
	std::map<std::string, std::string>::iterator iter;
	bool   statusOK = false;
	string response;
	
	int urlLen = url.size();

	int httpCode;
	for (iter = responses.begin(); iter != responses.end(); ++iter)
	{
		std::string key = iter->first;
		int pos = urlLen - key.size();
		if (pos > 0)
		{
			std::string urlTail = url.substr(pos, std::string::npos);
			if (urlTail == key)
			{
				response = iter->second;
				statusOK = true;
				httpCode = 200;
				break;
			}
		}
	}

	std::string errorMessage;
	std::shared_ptr<asyncHTTPResponse> result=std::make_shared<asyncHTTPResponse>();

	auto f = [ url, response, statusOK, httpCode, errorMessage]() {
		HTTPResponse result(url, response, statusOK, httpCode, errorMessage);
		return result;
	};


	result->futureResponse = std::async(f);
	return result;
}
} /* namespace Test */
