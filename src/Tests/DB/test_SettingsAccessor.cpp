#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "test_SettingsAccessor.h"
#include "TempDB.h"
#include "ISettingsAccessor.h"

namespace Test
{

void test_ShortFilterTimeAccessor()
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->SetShortFilterTime(30);

	ASSERT_EQUAL(30, settingsAccessor->GetShortFilterTime());
}

void test_IntAccessor()
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQUAL(10, settingsAccessor->GetIntByName("Tjohopp",10));
	settingsAccessor->SetIntByName("Tjohopp", 30);
	ASSERT_EQUAL(30, settingsAccessor->GetIntByName("Tjohopp",10));
}

void test_BoolAccessor()
{
	TempDB tempdb;
	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	ASSERT_EQUAL(true, settingsAccessor->GetBoolByName("Tjohopp",true));
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQUAL(false, settingsAccessor->GetBoolByName("Tjohopp",true));
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

void test_notification()
{
	TempDB tempdb;
	Observer observer;
	ASSERT_EQUAL("", observer.changed_setting);

	std::shared_ptr<ISettingsAccessor> settingsAccessor = tempdb.getSettingsAccessor();
	settingsAccessor->attach(&observer);
	settingsAccessor->SetBoolByName("Tjohopp", false);
	ASSERT_EQUAL("Tjohopp", observer.changed_setting);
	settingsAccessor->detach(&observer);

}

cute::suite make_suite_test_SettingsAccessor()
{
	cute::suite s;
	s.push_back(CUTE(test_ShortFilterTimeAccessor));
	s.push_back(CUTE(test_IntAccessor));
	s.push_back(CUTE(test_BoolAccessor));
	s.push_back(CUTE(test_notification));
	return s;
}

}

