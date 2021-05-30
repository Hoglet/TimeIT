/*
 * Settings.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <optional>
#include <libtimeit/db/database.h>
#include "sqlite3.h"

namespace libtimeit
{

using namespace std;

class Settings_accessor
{
	friend class Database;
public:
	Settings_accessor(Database& database);

	optional<int>   value(string name);

	int64_t   get_int(string name, int64_t default_value);
	bool      set_int(string name, int64_t default_value);

	bool      get_bool(string name, bool default_value);
	bool      set_bool(string name, bool value) ;

	string    get_string(string name, string default_value);
	bool      set_string(string name, string value);
protected:
	static void create_table(Database& db);

private:
	void  setting_changed(string name);

	Database& database;
	static void setup(Database& database);
	static void upgrade(Database& database);
};
}

#endif /* SETTINGS_H_ */
