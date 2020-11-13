#include <gtest/gtest.h>
#include <libtimeit/Timer.h>

class TimerTest: public TimerObserver
{
public:
	TimerTest()
	{
		oneSecond=0;
		tenSeconds=0;
		timer.attach(this);
	}

	void on_signal_1_second()
	{
		oneSecond++;
	};

	void on_signal_10_seconds()
	{
		tenSeconds++;
	};

	void tickTimer()
	{
		timer.on_signal_1_second();
	}

	int oneSecond;
	int tenSeconds;
private:
	Timer timer;
};


TEST( Timer, test1SecondTicks)
{
	TimerTest test;
	ASSERT_EQ(0,test.oneSecond);
	test.tickTimer();
	ASSERT_EQ(1,test.oneSecond);
}

TEST( Timer, test10SecondsTicks)
{
	TimerTest test;
	Timer timer;
	timer.attach(&test);
	ASSERT_EQ(0,test.tenSeconds);
	test.tickTimer();
	ASSERT_EQ(0,test.tenSeconds);
	for(int i=0;i<8;i++)
	{
		test.tickTimer();
	}
	ASSERT_EQ(1,test.tenSeconds);
}


