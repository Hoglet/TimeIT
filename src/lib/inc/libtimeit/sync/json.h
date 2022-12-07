#ifndef JSON_H
#define JSON_H
#include <string>
#include <jansson.h>
#include <list>
#include <vector>
#include <chrono>

namespace libtimeit
{
using namespace std;
using namespace std::chrono;

class json
{

public:
	json();
	~json();
	json(json&&);
	json(const json&);
	explicit json(json_t* op_object);
	explicit json(string  text);
	explicit json(int64_t number);
	explicit json(bool state);
	explicit json(seconds /*duration*/);
	explicit json(minutes /*duration*/);
	explicit json(time_point<system_clock> time_stamp);
	explicit json(vector<json>);
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

#endif //JSON_H
