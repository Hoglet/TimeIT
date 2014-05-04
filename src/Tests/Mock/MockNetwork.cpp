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

std::string MockNetwork::request(const std::string& url, std::string data, std::string username, std::string password, bool verifyPassword)
{
	std::map<std::string, std::string>::iterator iter;
	std::string returnValue;
	int urlLen = url.size();

	for (iter = responses.begin(); iter != responses.end(); ++iter)
	{
		std::string key = iter->first;
		std::string urlTail = url.substr(urlLen - key.size(), std::string::npos);
		if(urlTail == key)
		{
			returnValue = iter->second;
			break;
		}
	}

	return returnValue;
}
} /* namespace Test */
