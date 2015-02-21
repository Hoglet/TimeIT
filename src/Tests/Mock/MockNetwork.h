#ifndef MOCKNETWORK_H_
#define MOCKNETWORK_H_

#include "Network.h"

#include <map>

namespace Test
{

class MockNetwork: public INetwork
{
public:
	MockNetwork();
	virtual ~MockNetwork();
	virtual struct NetworkResponse request(const std::string& url, std::string data, std::string username, std::string password, bool verifyPassword);
	void setResponse(std::string& uri, std::string& response);
private:
	std::map<std::string, std::string> responses;
};

} /* namespace Test */
#endif /* MOCKNETWORK_H_ */
