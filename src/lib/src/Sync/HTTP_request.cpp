//
// Created by hoglet on 14/11/2020.
//

#include "libtimeit/sync/HTTP_request.h"
#include <cstring>
#include <stdexcept>

namespace libtimeit
{


size_t receive_data(void* ptr, size_t size, size_t nmemb, string* received_data)
{
	int length = size * nmemb;
	char data[length + 1];
	strncpy(data, (const char*) ptr, length);
	data[length] = 0;
	received_data->append(data);
	return size * nmemb;
}


HTTP_request::HTTP_request()
{
	curl = curl_easy_init();

	if (curl == nullptr)
	{
		throw runtime_error("Unable to initialise network");
	}
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_data);
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, send_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receive_buffer);
	curl_easy_setopt(curl, CURLOPT_READDATA, this);


}

void HTTP_request::ignore_cert_errors(bool ignore)
{
	ignore_certificate_errors = ignore;
}


size_t HTTP_request::send_data(void* pVoid, size_t i, size_t i1, HTTP_request* caller)
{
	size_t data_length = caller->send_buffer.size();
	size_t buffer_length = i * i1;
	size_t characters_to_send = data_length - caller->cur_send_position;
	if (characters_to_send > buffer_length)
	{
		characters_to_send = buffer_length;
	}

	strncpy((char*) pVoid, &(caller->send_buffer.c_str()[caller->cur_send_position]), characters_to_send);
	caller->cur_send_position += characters_to_send;
	return characters_to_send;

}

HTTP_request::~HTTP_request()
{
	curl_easy_cleanup(curl);
	curl = nullptr;
};

HTTP_response HTTP_request::PUT(
		string url,
		string data,
		string username,
		string password
)
{
	curl_easy_setopt(curl, CURLOPT_PUT, 1L);
	receive_buffer.clear();
	send_buffer = data;
	cur_send_position = 0;

	struct curl_slist* headers = NULL; // init to NULL is important
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long) CURLAUTH_BASIC);
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

	if (ignore_certificate_errors)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}
	/* Perform the request, res will get the return code */
	CURLcode res = curl_easy_perform(curl);

	int http_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

	HTTP_response result(
			url,
			receive_buffer,
			(res == CURLE_OK),
			http_code,
			curl_easy_strerror(res));

	curl_slist_free_all(headers);
	return result;
}
}