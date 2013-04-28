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
	}

	void Test()
	{

	}
	void test1sec()
	{
		oneSecond=0;
		timer.attach(this);
		ASSERT_EQUAL(0,oneSecond);
		timer.signalSender();
		ASSERT_EQUAL(1,oneSecond);
		timer.detach(this);
	}

	void test10sec()
	{
		tenSeconds=0;
		timer.attach(this);
		ASSERT_EQUAL(0,tenSeconds);
		timer.on_signal_1_second();
		ASSERT_EQUAL(0,tenSeconds);
		for(int i=0;i<8;i++)
		{
			timer.on_signal_1_second();
		}
		ASSERT_EQUAL(1,tenSeconds);
		timer.detach(this);
	}


	void on_signal_1_second()
	{
		oneSecond++;
	};

	void on_signal_10_seconds()
	{
		tenSeconds++;
	};

	Timer timer;
	int oneSecond;
	int tenSeconds;
};

void test1SecondTicks()
{
	TimerTest test;
	test.test1sec();
}
void test10SecondsTicks()
{
	TimerTest test;
	test.test10sec();
}

cute::suite make_suite_TimerTest(){
	cute::suite s;
	s.push_back(CUTE(test1SecondTicks));
	s.push_back(CUTE(test10SecondsTicks));
	return s;
}

}

