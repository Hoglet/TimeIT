/*
 * Settings.h
 *
 *  Created on: 2008-sep-06
 *      Author: hoglet
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <libtimeit/db/ISettingsAccessor.h>
#include "CSQL.h"

namespace libtimeit
{

class SettingsAccessor: public ISettingsAccessor
{
public:
	SettingsAccessor(std::shared_ptr<CSQL>& op_db);
	virtual ~SettingsAccessor();
	/* ShortFilterTime
	 * Time in minutes that is the shortest time counted as work.
	 * Shorter records is to be removed.
	 */
	virtual int GetShortFilterTime() ;
	virtual void SetShortFilterTime(int) ;

	virtual int GetIntByName(const std::string& name, int defaultValue);
	virtual bool SetIntByName(const std::string& name, int value);

	virtual bool GetBoolByName(const std::string& name, bool defaultValue);
	virtual bool SetBoolByName(const std::string& name, bool value) ;

	virtual std::string GetStringByName(const std::string& name, const std::string& defaultValue);
	virtual bool SetStringByName(const std::string& name, const std::string& value);


	virtual void attach(ISettingsAccessorObserver* observer);
	virtual void detach(ISettingsAccessorObserver* observer);
private:
	SettingsAccessor();
	void SettingsChanged(const std::string& name);
	int idleGT;
	int idleGZ;
	int shortFilterTime;
	std::shared_ptr<CSQL> db;
	std::list<ISettingsAccessorObserver*> observers;
};
}

#endif /* SETTINGS_H_ */
