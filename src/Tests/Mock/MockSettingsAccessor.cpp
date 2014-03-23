/*
 * MockSettingsAcessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockSettingsAccessor.h"

using namespace DB;

namespace Test
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

int MockSettingsAccessor::GetIntByName(const std::string& name, int defaultValue)
{
	return 1;
}

bool MockSettingsAccessor::SetIntByName(const std::string& name, int value)
{
	return false;
}

bool MockSettingsAccessor::GetBoolByName(const std::string& name, bool defaultValue)
{
	return false;
}

bool MockSettingsAccessor::SetBoolByName(const std::string& name, bool value)
{
	return false;
}

std::string MockSettingsAccessor::GetStringByName(const std::string& name, const std::string& defaultValue)
{
	return defaultValue;
}

bool MockSettingsAccessor::SetStringByName(const std::string& name, const std::string& value)
{
	return false;
}


} /* namespace Test */
