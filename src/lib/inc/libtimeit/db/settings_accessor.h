/*
 * Settings.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <libtimeit/db/database.h>
#include "CSQL.h"

namespace libtimeit
{

using namespace std;

class Settings_accessor_observer
{
public:
	virtual ~Settings_accessor_observer() = default;
	virtual void on_settings_changed( string name) = 0;
};

class Settings_accessor
{
public:
	Settings_accessor(Database& database);
	virtual ~Settings_accessor();

	int    get_int(string name, int default_value);
	bool   set_int(string name, int value);

	bool   get_bool(string name, bool default_value);
	bool   set_bool(string name, bool value) ;

	string get_string(string name, string default_value);
	bool   set_string(string name, string value);


	virtual void attach(Settings_accessor_observer* observer);
	virtual void detach(Settings_accessor_observer* observer);
private:
	Settings_accessor();
	void  setting_changed(string name);

	Database& database;
	list<Settings_accessor_observer*> observers;
};
}

#endif /* SETTINGS_H_ */
