#include <gtest/gtest.h>
#include "TempDB.h"
#include <DB/ISettingsAccessor.h>

using namespace DB;

TEST( SettingsAccessor, ShortFilterTimeAccessor)
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->SetShortFilterTime(30);

	ASSERT_EQ(30, settingsAccessor->GetShortFilterTime());
}

TEST( SettingsAccessor, IntAccessor)
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQ(10, settingsAccessor->GetIntByName("Tjohopp",10));
	settingsAccessor->SetIntByName("Tjohopp", 30);
	ASSERT_EQ(30, settingsAccessor->GetIntByName("Tjohopp",10));
}

TEST( SettingsAccessor, BoolAccessor)
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQ(true, settingsAccessor->GetBoolByName("Tjohopp",true));
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQ(false, settingsAccessor->GetBoolByName("Tjohopp",true));
}

class Observer: public ISettingsAccessorObserver
{
public:

	void on_settingsChanged(const std::string& name)
	{
		changed_setting = name;
	}

	std::string changed_setting;
};

TEST( SettingsAccessor, notification )
{
	TempDB tempdb;
	Observer observer;
	ASSERT_EQ("", observer.changed_setting);

	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->attach(&observer);
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQ("Tjohopp", observer.changed_setting);
	settingsAccessor->detach(&observer);

}


