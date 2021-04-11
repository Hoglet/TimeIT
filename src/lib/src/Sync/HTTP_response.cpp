//
// Created by hoglet on 16/11/2020.
//
#include <libtimeit/sync/HTTP_response.h>

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
		url(url_),
		response(response_),
		status_OK(status_OK_),
		http_code(http_code_),
		error_message(error_message_)
{
}

}