/*
 * X11_IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef X_11_IDLE_DETECTOR_H
#define X_11_IDLE_DETECTOR_H


#include <libtimeit/timer.h>
#include <ctime>
#include <memory>
#include <libtimeit/db/notifier.h>

namespace libtimeit
{

class x_11_idle_detector : public timer_observer
{
public:
	static bool available();
	x_11_idle_detector( timer_base &timer);
	x_11_idle_detector( const x_11_idle_detector&) = delete;
	x_11_idle_detector( x_11_idle_detector&&)      = delete;
	x_11_idle_detector& operator=( const x_11_idle_detector&) = delete;
	x_11_idle_detector& operator=( x_11_idle_detector&& ) = delete;

	~x_11_idle_detector() override;
	seconds time_idle();
	bool idle();
private:
	void   poll_status();
	seconds  idle_seconds = 0s;
	time_point<system_clock>  last_poll { system_clock::from_time_t(0) };
	time_point<system_clock>   last_activity;
	seconds    idle_timeout{2000}; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	bool     is_idle{false};
};

}
#endif /* X_11_IDLE_DETECTOR_H */
