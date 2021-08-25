#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

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
			string op_url,
			string op_response,
			bool   op_status_ok,
			int    op_http_code,
			string op_error_message
	);
};

}
#endif //HTTP_RESPONSE_H
