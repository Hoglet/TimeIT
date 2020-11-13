/*
 * ISettingsAccessor.h
 *
 *  Created on: Mar 25, 2009
 *      Author: hoglet
 */

#ifndef ISETTINGSACCESSOR_H_
#define ISETTINGSACCESSOR_H_

#include <string>

namespace DB
{

class ISettingsAccessorObserver
{
public:
	virtual ~ISettingsAccessorObserver();
	virtual void on_settingsChanged(const std::string& name) = 0;
};

class ISettingsAccessor
{
public:
	virtual ~ISettingsAccessor();
	/* ShortFilterTime
	 * Time in minutes that is the shortest time counted as work.
	 * Shorter records is to be removed.
	 */
	virtual int GetShortFilterTime() =0;
	virtual void SetShortFilterTime(int) =0;



	virtual void attach(ISettingsAccessorObserver* ) = 0;
	virtual void detach(ISettingsAccessorObserver* ) = 0;

	virtual int GetIntByName(const std::string& name, int defaultValue) = 0;
	virtual bool SetIntByName(const std::string& name, int value) = 0;

	virtual bool GetBoolByName(const std::string& name, bool defaultValue) = 0;
	virtual bool SetBoolByName(const std::string& name, bool value) = 0;

	virtual std::string GetStringByName(const std::string& name, const std::string& defaultValue) = 0;
	virtual bool SetStringByName(const std::string& name, const std::string& value) = 0;
};
}

#endif /* ISETTINGSACCESSOR_H_ */
