#include "mock_network.h"

using namespace std;

namespace test
{

mock_network::mock_network()
{
	// TODO Auto-generated constructor stub

}

mock_network::~mock_network()
{
	// TODO Auto-generated destructor stub
}

void mock_network::set_response( string uri, string response)
{
	responses[uri] = response;
}

shared_ptr<async_http_response> mock_network::request(string url, string , string , string , bool )
{
	map<string, string>::iterator iter;
	bool   status_ok = false;
	string response;
	
	int url_len = url.size();

	int http_code=200;
	for (iter = responses.begin(); iter != responses.end(); ++iter)
	{
		string key = iter->first;
		int pos = url_len - key.size();
		if (pos > 0)
		{
			string url_tail = url.substr( pos, string::npos);
			if ( url_tail == key)
			{
				response = iter->second;
				status_ok = true;
				http_code = 200;
				break;
			}
		}
	}

	string error_message;
	shared_ptr<async_http_response> result=make_shared<async_http_response>();

	auto f = [ url, response, status_ok, http_code, error_message]() {
		return http_response( url, response, status_ok, http_code, error_message);
	};


	result->future_response = async(f);
	return result;
}
} /* namespace test */
