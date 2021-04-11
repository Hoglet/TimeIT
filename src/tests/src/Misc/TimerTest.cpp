#include <gtest/gtest.h>
#include <libtimeit/timer.h>

using namespace libtimeit;

class TimerTest: public Timer_observer
{
public:
	explicit TimerTest(Timer& timer) : Timer_observer(timer)
	{
		oneSecond=0;
		tenSeconds=0;
	}

	void on_signal_1_second() override
	{
		oneSecond++;
	};

	void on_signal_10_seconds() override
	{
		tenSeconds++;
	};


	int oneSecond;
	int tenSeconds;

};

TEST( Timer, test1SecondTicks) /* NOLINT */
{
	Timer     timer;
	TimerTest test(timer);

	ASSERT_EQ(0,test.oneSecond);
	timer.on_signal_1_second();
	ASSERT_EQ(1,test.oneSecond);
}

TEST( Timer, test10SecondsTicks) /* NOLINT */
{
	Timer     timer;
	TimerTest test(timer);
	ASSERT_EQ(0,test.tenSeconds);
	timer.on_signal_1_second();
	ASSERT_EQ(0,test.tenSeconds);
	for(int i=0;i<8;i++)
	{
		timer.on_signal_1_second();
	}
	ASSERT_EQ(1,test.tenSeconds);
}


