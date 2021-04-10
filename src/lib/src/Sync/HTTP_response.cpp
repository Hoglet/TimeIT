//
// Created by hoglet on 16/11/2020.
//
#include <libtimeit/sync/HTTP_response.h>

using namespace std;

string HTTPResponse::url()
{
	return url_;
}

string HTTPResponse::response()
{
	return response_;
}

bool   HTTPResponse::statusOK()
{
	return statusOK_;
}

int    HTTPResponse::httpCode()
{
	return httpCode_;
}
string HTTPResponse::errorMessage()
{
	return errorMessage_;
}

HTTPResponse::HTTPResponse(
		string url,
		string response,
		bool statusOK,
		int httpCode,
		string errorMessage
):
url_(url),
response_(response),
statusOK_(statusOK),
httpCode_(httpCode),
errorMessage_(errorMessage)
{

}

