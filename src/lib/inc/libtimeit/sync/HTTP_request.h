//
// Created by hoglet on 14/11/2020.
//
#ifndef TIMEIT_HTTP_REQUEST_H
#define TIMEIT_HTTP_REQUEST_H

#include <libtimeit/sync/HTTP_response.h>
#include <curl/curl.h>
#include <string>

namespace libtimeit
{
using namespace std;

class HTTP_request
{
public:
	HTTP_request();
	HTTP_request( const HTTP_request& ) = delete;
	HTTP_request( HTTP_request&& ) = delete;
	HTTP_request& operator=( const HTTP_request& ) = delete;
	HTTP_request& operator=( HTTP_request&& ) = delete;
	~HTTP_request();
	void ignore_cert_errors(bool ignore);
	HTTP_response put(
			string url,
			string data,
			string username,
			string password
			);
private:
	static size_t   send_data(void *pVoid, size_t i, size_t i1, HTTP_request* caller);

	CURL*    curl;
	string   receive_buffer;
	string   send_buffer;

	size_t   cur_send_position{0};
	bool     ignore_certificate_errors{false};
};

}
#endif //TIMEIT_HTTP_REQUEST_H
