#include <gtest/gtest.h>
#include "temp_db.h"
#include <libtimeit/db/settings_accessor.h>

namespace test
{
using namespace libtimeit;
using namespace std;


TEST(Settings_accessor, IntAccessor)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	settings_accessor settings( tempdb );
	ASSERT_EQ( 10, settings.get_int( "Tjohopp", 10));
	settings.set_int( "Tjohopp", 30);
	ASSERT_EQ( 30, settings.get_int( "Tjohopp", 10));
}

TEST(Settings_accessor, BoolAccessor)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	settings_accessor settings( tempdb);
	ASSERT_EQ( true, settings.get_bool( "Tjohopp", true));
	settings.set_bool( "Tjohopp", false);
	ASSERT_EQ( false, settings.get_bool( "Tjohopp", true));
}

class observer : public event_observer
{
public:
	observer( notification_manager& notifier) : event_observer( notifier) {};
	void on_settings_changed(string name)
	{
		changed_setting = name;
	}

	std::string changed_setting;
};

TEST(Settings_accessor, notification)
{
	notification_manager notifier;
	temp_db tempdb(notifier);
	observer observer( notifier);
	ASSERT_EQ("", observer.changed_setting);
	settings_accessor settings( tempdb);
	settings.set_bool( "Tjohopp", false);
	ASSERT_EQ("Tjohopp", observer.changed_setting);

}


}
