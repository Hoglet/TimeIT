/*
 * Settings.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <optional>
#include <libtimeit/db/accessor.h>
#include <libtimeit/db/database.h>
#include "sqlite3.h"

namespace libtimeit
{

using namespace std;

class Settings_accessor : public Accessor
{
	friend class Database;
public:
	Settings_accessor(Database& database);
	virtual ~Settings_accessor();

	optional<int>   value(string name);

	int    get_int(string name, int default_value);
	bool   set_int(string name, int value);

	bool   get_bool(string name, bool default_value);
	bool   set_bool(string name, bool value) ;

	string get_string(string name, string default_value);
	bool   set_string(string name, string value);
protected:
	void      create_table()   override;
	void      drop_views()     override {};
	void      create_views()   override {};
	void      upgrade()        override;

private:
	void  setting_changed(string name);

	Database& database;
};
}

#endif /* SETTINGS_H_ */
