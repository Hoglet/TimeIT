#ifndef MOCK_NETWORK_H
#define MOCK_NETWORK_H

#include "libtimeit/sync/network.h"

#include <map>

namespace test
{
using namespace libtimeit;

class mock_network: public abstract_network
{
public:
	mock_network();
	virtual ~mock_network();

	shared_ptr<async_http_response> request(
			string url,
			string data,
			string username,
			string password,
			bool   verify_password) override;

	void set_response( string uri, string response);
private:
	map<string, string> responses;
};

} /* namespace test */
#endif /* MOCK_NETWORK_H */
