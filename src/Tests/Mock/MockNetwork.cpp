#include "MockNetwork.h"

using namespace std;

namespace Test
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

struct NetworkResponse MockNetwork::request(const std::string& url, std::string data, std::string username, std::string password,
		bool verifyPassword)
{
	std::map<std::string, std::string>::iterator iter;
	NetworkResponse returnValue;
	returnValue.statusOK = false;
	returnValue.url = url;
	int urlLen = url.size();

	for (iter = responses.begin(); iter != responses.end(); ++iter)
	{
		std::string key = iter->first;
		int pos = urlLen - key.size();
		if (pos > 0)
		{
			std::string urlTail = url.substr(pos, std::string::npos);
			if (urlTail == key)
			{
				returnValue.response = iter->second;
				returnValue.statusOK = true;
				returnValue.httpCode = 200;
				break;
			}
		}
	}

	return returnValue;
}
} /* namespace Test */
