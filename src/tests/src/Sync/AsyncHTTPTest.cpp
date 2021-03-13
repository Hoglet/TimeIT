#include <gtest/gtest.h>
#include "libtimeit/sync/Network.h"

namespace test
{
using namespace std;



bool requestDone(shared_future <HTTPResponse> future)
{
	if(future.valid())
	{
		return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}
	else
	{
		return false;
	}
}

void doStandard()
{
	HTTPRequest request;
	request.PUT("https://timeit.solit.se", "", "nobody", "wrong");
}

void doAsync(Network network)
{
	HTTPRequest request;
	auto result =network.request("https://timeit.solit.se", "", "nobody", "wrong", false);

	while ( !requestDone(result->futureResponse))
	{
		;
	}
}

TEST( AsyncNetwork, crashTest )
{
	Network network;
	doStandard();
	doStandard();
	doAsync(network);
	doAsync(network);
}


} /* namespace test */
