//
// Created by hoglet on 16/11/2020.
//
#ifndef TIMEIT_HTTP_RESPONSE_H
#define TIMEIT_HTTP_RESPONSE_H

#include <string>

namespace libtimeit
{
using namespace std;

class http_request;

struct http_response
{
	const string url;
	const string response;
	const bool   status_ok;
	const int    http_code;
	const string error_message;

	http_response(
			string url,
			string response,
			bool   status_OK,
			int    http_code_,
			string error_message_
	);
};

}
#endif //TIMEIT_HTTP_RESPONSE_H
