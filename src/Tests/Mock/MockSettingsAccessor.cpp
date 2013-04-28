/*
 * MockSettingsAcessor.cpp
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#include "MockSettingsAccessor.h"

namespace Test
{

MockSettingsAccessor::MockSettingsAccessor()
{
	// TODO Auto-generated constructor stub

}

MockSettingsAccessor::~MockSettingsAccessor()
{
	// TODO Auto-generated destructor stub
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


} /* namespace Test */
