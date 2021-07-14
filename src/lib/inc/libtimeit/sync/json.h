

#ifndef TIMEIT_JSON_H
#define TIMEIT_JSON_H
#include <string>
#include <jansson.h>
#include <list>
#include <vector>

namespace libtimeit
{
using namespace std;

class json
{

public:
	json();
	~json();
	json(json&&);
	json(const json&);
	json(json_t* object);
	json(string  text);
	json(int64_t number);
	json(bool state);
	json(vector<json>);
	json& operator=(json&&);
	json& operator=(const json&) = delete;

	static json from_json_string(string text);

	void set(string key, json value);

	[[nodiscard]] vector<json> objects()           const;
	[[nodiscard]] string       text(string key)    const;
	[[nodiscard]] bool         boolean(string key) const;
	[[nodiscard]] int64_t      integer(string key) const;
	[[nodiscard]] json         by_name(string key) const;


	string to_string();
private:
	json_t*      object = nullptr;
};

}

#endif //TIMEIT_JSON_H
