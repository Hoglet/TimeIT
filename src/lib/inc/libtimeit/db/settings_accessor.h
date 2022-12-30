#ifndef SETTINGS_ACCESSOR_H
#define SETTINGS_ACCESSOR_H

#include <optional>
#include <libtimeit/db/database.h>
#include "sqlite.h"

namespace libtimeit
{

using namespace std;

class settings_accessor
{
	friend class database;
public:
	settings_accessor(database& database);

	optional<int>   value(string name);

	int64_t   get_int(string name, int64_t default_value);
	bool      set_int(string name, int64_t default_value);

	bool      get_bool(string name, bool default_value);
	bool      set_bool(string name, bool value) ;

	string    get_string(string_view name, string_view default_value);
	bool      set_string(string_view name, string_view value);
protected:
	static void create_table(database& db);

private:
	void  setting_changed(string_view name);

	database& db;
	static void setup(database& db);
	static void upgrade(database& database);
};
}

#endif /* SETTINGS_ACCESSOR_H */
