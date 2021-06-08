#include "libtimeit/sync/HTTP_request.h"
#include <cstring>
#include <stdexcept>

namespace libtimeit
{

size_t receive_data(char* ptr, size_t size, size_t nmemb, stringstream* received_data)
{
	auto length = size * nmemb;
	(*received_data).write(ptr, length);
	return size * nmemb;
}


HTTP_request::HTTP_request()
{
	curl = curl_easy_init(); // NOLINT

	if (curl == nullptr)
	{
		throw runtime_error("Unable to initialise network");
	}
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);           // NOLINT
//	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);                          // NOLINT
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive_data);        // NOLINT
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, send_data);            // NOLINT
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &receive_buffer);  // NOLINT
	curl_easy_setopt(curl, CURLOPT_READDATA, this);                    // NOLINT


}

void HTTP_request::ignore_cert_errors(bool ignore)
{
	ignore_certificate_errors = ignore;
}


size_t HTTP_request::send_data(char* dest, size_t i, size_t i1, HTTP_request* caller)
{
	size_t data_length = caller->send_buffer.size();
	size_t buffer_length = i * i1;
	size_t characters_to_send = data_length - caller->cur_send_position;
	if (characters_to_send > buffer_length)
	{
		characters_to_send = buffer_length;
	}
	auto read_pointer=&(caller->send_buffer.c_str()[caller->cur_send_position]);
	memcpy(dest, read_pointer , characters_to_send); // NOLINT
	caller->cur_send_position += characters_to_send;
	return characters_to_send;

}

HTTP_request::~HTTP_request()
{
	curl_easy_cleanup(curl);
	curl = nullptr;
}

HTTP_response HTTP_request::put(
		string url,
		string data,
		string username,
		string password
)
{
	curl_easy_setopt(curl, CURLOPT_PUT, 1L); // NOLINT
	receive_buffer.clear();
	send_buffer = data;
	cur_send_position = 0;

	struct curl_slist* headers = nullptr; // init to NULL is important
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);                                  // NOLINT
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());                              // NOLINT
	curl_easy_setopt(curl, CURLOPT_USERPWD, (username + ":" + password).c_str());  // NOLINT
	curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long) CURLAUTH_BASIC);               // NOLINT
	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);                                    // NOLINT

	if (ignore_certificate_errors)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);                        // NOLINT
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);                        // NOLINT
	}
	/* Perform the request, res will get the return code */
	CURLcode res = curl_easy_perform(curl);

	int http_code{0};
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);             // NOLINT

	HTTP_response result(
			url,
			receive_buffer.str(),
			(res == CURLE_OK),
			http_code,
			curl_easy_strerror(res));

	curl_slist_free_all(headers);
	return result;
}
}