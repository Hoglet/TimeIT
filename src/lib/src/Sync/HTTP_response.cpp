//
// Created by hoglet on 16/11/2020.
//
#include <libtimeit/sync/HTTP_response.h>

#include <utility>

namespace libtimeit
{


using namespace std;

HTTP_response::HTTP_response(
			string url_,
			string response_,
			bool   status_OK_,
			int    http_code_,
			string error_message_
		) :
		url(std::move(url_)),
		response(std::move(response_)),
		status_ok(status_OK_),
		http_code(http_code_),
		error_message(std::move(error_message_))
{
}

}