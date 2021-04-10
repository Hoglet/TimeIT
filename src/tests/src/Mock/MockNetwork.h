#ifndef MOCKNETWORK_H_
#define MOCKNETWORK_H_

#include "libtimeit/sync/network.h"

#include <map>

namespace test
{
using namespace libtimeit;

class MockNetwork: public INetwork
{
public:
	MockNetwork();
	virtual ~MockNetwork();

	virtual shared_ptr<asyncHTTPResponse> request(
			string url,
			string data,
			string username,
			string password,
			bool   verifyPassword);

	void setResponse( string uri, string response);
private:
	map<string, string> responses;
};

} /* namespace Test */
#endif /* MOCKNETWORK_H_ */
