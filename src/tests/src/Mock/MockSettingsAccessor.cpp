/*
 * MockSettingsAcessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockSettingsAccessor.h"

using namespace libtimeit;
using namespace std;

namespace test
{

MockSettingsAccessor::MockSettingsAccessor()
{
}

MockSettingsAccessor::~MockSettingsAccessor()
{
}

int MockSettingsAccessor::GetShortFilterTime()
{
	return 60;
}
void MockSettingsAccessor::SetShortFilterTime(int)
{
}



void MockSettingsAccessor::attach(ISettingsAccessorObserver* )
{
}

void MockSettingsAccessor::detach(ISettingsAccessorObserver* )
{
}

int MockSettingsAccessor::GetIntByName(const string& name, int defaultValue)
{
	return 1;
}

bool MockSettingsAccessor::SetIntByName(const string& name, int value)
{
	return false;
}

bool MockSettingsAccessor::GetBoolByName(const string& name, bool defaultValue)
{
	return false;
}

bool MockSettingsAccessor::SetBoolByName(const string& name, bool value)
{
	return false;
}

string MockSettingsAccessor::GetStringByName(const string& name, const string& defaultValue)
{
	return defaultValue;
}

bool MockSettingsAccessor::SetStringByName(const string& name, const string& value)
{
	return false;
}


} /* namespace Test */
