//
// Created by hoglet on 16/11/2020.
//
#include <libtimeit/sync/http_response.h>

#include <utility>

namespace libtimeit
{


using namespace std;

http_response::http_response(
			string op_url,
			string op_response,
			bool   op_status_ok,
			int    op_http_code,
			string op_error_message
		) :
		url(std::move( op_url)),
		response(std::move( op_response)),
		status_ok( op_status_ok),
		http_code( op_http_code),
		error_message(std::move( op_error_message))
{
}

}