

#ifndef TIMEIT_JSON_H
#define TIMEIT_JSON_H
#include <string>
#include <jansson.h>
#include <list>
#include <vector>

namespace libtimeit
{
using namespace std;

class Json
{

public:
	Json();
	~Json();
	Json(Json&&);
	Json(const Json&);
	Json(json_t* object);
	Json(string  text);
	Json(int64_t number);
	Json(bool state);
	Json(vector<Json>);
	Json& operator=(Json&&);
	Json& operator=(const Json&) = delete;

	static Json from_json_string(string text);

	void set(string key, Json value);

	[[nodiscard]] vector<Json> objects()           const;
	[[nodiscard]] string       text(string key)    const;
	[[nodiscard]] bool         boolean(string key) const;
	[[nodiscard]] int64_t      integer(string key) const;
	[[nodiscard]] Json         by_name(string key) const;


	string to_string();
private:
	json_t*      object = nullptr;
};

}

#endif //TIMEIT_JSON_H
