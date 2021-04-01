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


private:
	Settings_accessor();
	void  setting_changed(string name);

	Database& database;
};
}

#endif /* SETTINGS_H_ */
