#include <gtest/gtest.h>
#include <libtimeit/timer.h>

using namespace libtimeit;

class timer_test: public timer_observer
{
public:
	explicit timer_test( timer_base& timer) : timer_observer( timer)
	{
		one_second=0;
		ten_seconds=0;
	}

	void on_signal_1_second() override
	{
		one_second++;
	};

	void on_signal_10_seconds() override
	{
		ten_seconds++;
	};


	int one_second;
	int ten_seconds;

};

TEST( timer_base, test1SecondTicks) /* NOLINT */
{
	timer_base     timer;
	timer_test test(timer);

	ASSERT_EQ(0,test.one_second);
	timer.on_signal_1_second();
	ASSERT_EQ(1,test.one_second);
}

TEST( timer_base, test10SecondsTicks) /* NOLINT */
{
	timer_base     timer;
	timer_test test(timer);
	ASSERT_EQ(0,test.ten_seconds);
	timer.on_signal_1_second();
	ASSERT_EQ(0,test.ten_seconds);
	for(int i=0;i<9;i++)
	{
		timer.on_signal_1_second();
	}
	ASSERT_EQ(1,test.ten_seconds);
}
