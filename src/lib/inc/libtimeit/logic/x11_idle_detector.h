/*
 * X11_IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef IDLE_DETECTOR_H_
#define IDLE_DETECTOR_H_


#include <libtimeit/timer.h>
#include <ctime>
#include <memory>
#include <libtimeit/db/notifier.h>

namespace libtimeit
{

class X11_idle_detector : public timer_observer
{
public:
	static bool available();
	X11_idle_detector(Timer &timer);
	X11_idle_detector(const X11_idle_detector&) = delete;
	X11_idle_detector(X11_idle_detector&&)      = delete;
	X11_idle_detector& operator=(const X11_idle_detector&) = delete;
	X11_idle_detector& operator=( X11_idle_detector&& ) = delete;

	~X11_idle_detector() override;
	time_t time_idle();
	bool idle();
private:
	void   poll_status();
	time_t  idle_seconds = 0;
	time_t  last_poll = 0;
	long    idle_timeout{2000}; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	bool     is_idle{false};
	time_t   last_activity;
};

}
#endif /* IDLE_DETECTOR_H_ */
