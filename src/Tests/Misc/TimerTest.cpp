#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "TimerTest.h"
#include "Timer.h"

namespace Test
{

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


void Timer_test1SecondTicks()
{
	TimerTest test;
	ASSERT_EQUAL(0,test.oneSecond);
	test.tickTimer();
	ASSERT_EQUAL(1,test.oneSecond);
}

void Timer_test10SecondsTicks()
{
	TimerTest test;
	Timer timer;
	timer.attach(&test);
	ASSERT_EQUAL(0,test.tenSeconds);
	test.tickTimer();
	ASSERT_EQUAL(0,test.tenSeconds);
	for(int i=0;i<8;i++)
	{
		test.tickTimer();
	}
	ASSERT_EQUAL(1,test.tenSeconds);
}

cute::suite make_suite_TimerTest(){
	cute::suite s;
	s.push_back(CUTE(Timer_test1SecondTicks));
	s.push_back(CUTE(Timer_test10SecondsTicks));
	return s;
}

}

