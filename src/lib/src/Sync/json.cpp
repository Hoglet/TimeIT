

#include <libtimeit/sync/json.h>
#include <list>
#include <iostream>

namespace libtimeit
{
using namespace std::chrono;

json::json() : object(json_object())
{
}

json::json(string text) : object(json_string(text.c_str()))
{
}

json::json(time_point<system_clock> time_stamp) : object(json_integer(system_clock::to_time_t(time_stamp)))
{
}

json::json(seconds duration): object(json_integer(duration.count()))
{
}

json::json(minutes duration): object(json_integer(duration.count()))
{
}


json::json(int64_t number) : object(json_integer(number))
{
}

json::json(bool is_true)
{
	if(is_true)
	{
		object=json_true();
	}
	else
	{
		object=json_false();
	}
}


json json::from_json_string(string text)
{

	json_error_t error{};
	json return_value(json_loads(text.c_str(), 0, &error));

	if(json_error_code(&error) != json_error_unknown)
	{
		constexpr const size_t SHOW_BEFORE = 80;
		constexpr const size_t LENGTH_TO_SHOW  = SHOW_BEFORE + 80;
		cerr << "Error parsing string: "  << json_error_code(&error)  << ", "<< (const char*)error.text << "\n";
		cerr << "On position: " << error.position << " (line: " << error.line << " , column: " << error.column << "\n";
		cerr << "source: \n";
		size_t pos_start= error.position - SHOW_BEFORE;
		cerr << text.substr( pos_start, LENGTH_TO_SHOW)<< "\n\n";
	}

	return return_value;
}


json::json(json&& other) : object(std::move(other.object))
{
}

json::json(const json& other) : object(other.object)
{
	json_incref(object);
}

json& json::operator=(json&& other)
{
	json_decref(object);
	object = std::move(other.object);
	return *this;
}

json::json(json_t* object_) : object(object_)
{
	json_incref(object);
}

json::~json()
{
	json_decref(object);
}

void json::set(string key, json value)
{
	json_object_set(object, key.c_str(), value.object);
}


vector<json> json::objects() const
{
	vector<json> return_value{};
	for (size_t i = 0; i < json_array_size(object); i++)
	{
		json_t *item = json_array_get(object, i);
		return_value.push_back(json(item));
	}
	return return_value;
}

string  json::text(string key) const
{
	string return_value{};

	json_t *value  = json_object_get(object, key.c_str());
	if (json_is_string(value))
	{
		return_value = json_string_value(value);
	}
	return return_value;
}

bool json::boolean(string key) const
{
	bool return_value{false};

	json_t *value  = json_object_get(object, key.c_str());
	if (json_is_boolean(value))
	{
		return_value = json_is_true(value);
	}
	return return_value;
}

int64_t json::integer(string key) const
{
	int64_t return_value = 0;

	json_t *value  = json_object_get(object, key.c_str());
	if (json_is_integer(value))
	{
		return_value = json_integer_value(value);
	}
	return return_value;
}

json json::by_name(string key) const
{
	json_t *value  = json_object_get(object, key.c_str());
	if (json_is_object(value))
	{
		return json(value);
	}
	return json(nullptr);
}

json::json(vector<json> items) : object(json_array())
{
	for( auto item: items)
	{
		json_array_append(object, item.object);
	}
}

string json::to_string()
{
	string result;
	char *str = json_dumps(object, 0);    // NOLINT
	if (str != nullptr)
	{
		result = str;
		free(str);    // NOLINT
	}
	return result;
}

}