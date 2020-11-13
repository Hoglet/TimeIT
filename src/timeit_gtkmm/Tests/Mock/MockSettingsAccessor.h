/*
 * MockSettingsAcessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKSETTINGSACESSOR_H_
#define MOCKSETTINGSACESSOR_H_

#include <DB/ISettingsAccessor.h>

namespace Test
{

class MockSettingsAccessor: public DB::ISettingsAccessor
{
public:
	MockSettingsAccessor();
	virtual ~MockSettingsAccessor();

	virtual int GetShortFilterTime();
	virtual void SetShortFilterTime(int);

	virtual void attach(DB::ISettingsAccessorObserver* );
	virtual void detach(DB::ISettingsAccessorObserver* );

	virtual int GetIntByName(const std::string& name, int defaultValue);
	virtual bool SetIntByName(const std::string& name, int value);

	virtual bool GetBoolByName(const std::string& name, bool defaultValue);
	virtual bool SetBoolByName(const std::string& name, bool value);

	virtual std::string GetStringByName(const std::string& name, const std::string& defaultValue);
	virtual bool SetStringByName(const std::string& name, const std::string& value);

};

} /* namespace Test */
#endif /* MOCKSETTINGSACESSOR_H_ */
