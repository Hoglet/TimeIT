/*
 * MockSettingsAcessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKSETTINGSACESSOR_H_
#define MOCKSETTINGSACESSOR_H_

#include <libtimeit/db/ISettingsAccessor.h>

namespace Test
{
using namespace libtimeit;
using namespace std;

class MockSettingsAccessor: public ISettingsAccessor
{
public:
	MockSettingsAccessor();
	virtual ~MockSettingsAccessor();

	virtual int GetShortFilterTime();
	virtual void SetShortFilterTime(int);

	virtual void attach(ISettingsAccessorObserver* );
	virtual void detach(ISettingsAccessorObserver* );

	virtual int GetIntByName(const string& name, int defaultValue);
	virtual bool SetIntByName(const string& name, int value);

	virtual bool GetBoolByName(const string& name, bool defaultValue);
	virtual bool SetBoolByName(const string& name, bool value);

	virtual string GetStringByName(const string& name, const string& defaultValue);
	virtual bool SetStringByName(const string& name, const string& value);

};

} /* namespace Test */
#endif /* MOCKSETTINGSACESSOR_H_ */
