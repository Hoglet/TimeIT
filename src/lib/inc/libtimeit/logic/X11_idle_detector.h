/*
 * X11_IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef IDLEDETECTOR_H_
#define IDLEDETECTOR_H_
#include <libtimeit/timer.h>
#include <time.h>
#include <memory>
#include <libtimeit/db/notifier.h>

namespace libtimeit
{


class X11_idle_detector : public Timer_observer
{
public:
	static bool available();
	X11_idle_detector(Timer &timer);
	virtual ~X11_idle_detector();
	int    minutes_idle();
	time_t time_idle();
	void idle_timeout(unsigned minutes);
	void reset();
	bool idle();
private:
	void   poll_status();

	time_t idle_seconds;
	time_t last_poll = 0;
	long   idle_timeout_;
	bool   is_idle;
	time_t last_activity;
};

}
#endif /* IDLEDETECTOR_H_ */
