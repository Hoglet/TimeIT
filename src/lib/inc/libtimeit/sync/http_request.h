//
// Created by hoglet on 14/11/2020.
//
#ifndef TIMEIT_HTTP_REQUEST_H
#define TIMEIT_HTTP_REQUEST_H

#include <libtimeit/sync/http_response.h>
#include <curl/curl.h>
#include <string>
#include <sstream>

namespace libtimeit
{
using namespace std;

class http_request
{
public:
	http_request();
	http_request( const http_request& ) = delete;
	http_request( http_request&& ) = delete;
	http_request& operator=( const http_request& ) = delete;
	http_request& operator=( http_request&& ) = delete;
	~http_request();
	void ignore_cert_errors(bool ignore);
	http_response put(
			string url,
			string data,
			string username,
			string password
			);
private:
	static size_t   send_data(char* dest, size_t i, size_t i1, http_request* caller);

	CURL*        curl;
	stringstream receive_buffer;
	string       send_buffer;

	size_t   cur_send_position{0};
	bool     ignore_certificate_errors{false};
};

}
#endif //TIMEIT_HTTP_REQUEST_H
