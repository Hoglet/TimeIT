#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/ISettingsAccessor.h>

namespace test
{
using namespace libtimeit;
using namespace std;

TEST(SettingsAccessor, ShortFilterTimeAccessor)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->SetShortFilterTime(30);

	ASSERT_EQ(30, settingsAccessor->GetShortFilterTime());
}

TEST(SettingsAccessor, IntAccessor)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQ(10, settingsAccessor->GetIntByName("Tjohopp", 10));
	settingsAccessor->SetIntByName("Tjohopp", 30);
	ASSERT_EQ(30, settingsAccessor->GetIntByName("Tjohopp", 10));
}

TEST(SettingsAccessor, BoolAccessor)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQ(true, settingsAccessor->GetBoolByName("Tjohopp", true));
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQ(false, settingsAccessor->GetBoolByName("Tjohopp", true));
}

class Observer : public ISettingsAccessorObserver
{
public:

	void on_settingsChanged(const std::string &name)
	{
		changed_setting = name;
	}

	std::string changed_setting;
};

TEST(SettingsAccessor, notification)
{
	Notifier notifier;
	TempDB tempdb(notifier);
	Observer observer;
	ASSERT_EQ("", observer.changed_setting);

	shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->attach(&observer);
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQ("Tjohopp", observer.changed_setting);
	settingsAccessor->detach(&observer);

}


}