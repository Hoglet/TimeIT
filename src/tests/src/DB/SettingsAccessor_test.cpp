#include <gtest/gtest.h>
#include "TempDB.h"
#include <libtimeit/db/settings_accessor.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST(Settings_accessor, IntAccessor)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	settings_accessor settingsAccessor(tempdb );
	ASSERT_EQ(10, settingsAccessor.get_int("Tjohopp", 10));
	settingsAccessor.set_int("Tjohopp", 30);
	ASSERT_EQ(30, settingsAccessor.get_int("Tjohopp", 10));
}

TEST(Settings_accessor, BoolAccessor)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	settings_accessor settingsAccessor(tempdb);
	ASSERT_EQ(true, settingsAccessor.get_bool("Tjohopp", true));
	settingsAccessor.set_bool("Tjohopp", false);
	ASSERT_EQ(false, settingsAccessor.get_bool("Tjohopp", true));
}

class Observer : public event_observer
{
public:
	Observer(notification_manager& notifier) : event_observer(notifier) {};
	void on_settings_changed(string name)
	{
		changed_setting = name;
	}

	std::string changed_setting;
};

TEST(Settings_accessor, notification)
{
	notification_manager notifier;
	TempDB tempdb(notifier);
	Observer observer(notifier);
	ASSERT_EQ("", observer.changed_setting);
	settings_accessor settingsAccessor(tempdb);
	settingsAccessor.set_bool("Tjohopp", false);
	ASSERT_EQ("Tjohopp", observer.changed_setting);

}


}