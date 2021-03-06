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

void MockNetwork::setResponse(string uri, string response)
{
	responses[uri] = response;
}

shared_ptr<asyncHTTPResponse> MockNetwork::request(string url, string , string , string ,
		bool )
{
	map<string, string>::iterator iter;
	bool   statusOK = false;
	string response;
	
	int urlLen = url.size();

	int httpCode;
	for (iter = responses.begin(); iter != responses.end(); ++iter)
	{
		string key = iter->first;
		int pos = urlLen - key.size();
		if (pos > 0)
		{
			string urlTail = url.substr(pos, string::npos);
			if (urlTail == key)
			{
				response = iter->second;
				statusOK = true;
				httpCode = 200;
				break;
			}
		}
	}

	string errorMessage;
	shared_ptr<asyncHTTPResponse> result=make_shared<asyncHTTPResponse>();

	auto f = [ url, response, statusOK, httpCode, errorMessage]() {
		return HTTP_response(url, response, statusOK, httpCode, errorMessage);
	};


	result->future_response = async(f);
	return result;
}
} /* namespace Test */
