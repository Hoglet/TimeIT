/*
 * X11_IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef X11_IDLE_DETECTOR_H
#define X11_IDLE_DETECTOR_H

#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <libtimeit/timer.h>
#include <ctime>
#include <memory>
#include <libtimeit/db/notifier.h>

#include "idle_detector.h"

namespace libtimeit
{

class x11_idle_detector : public idle_detector
{
public:
	bool available();
	x11_idle_detector( );
	x11_idle_detector( const x11_idle_detector&) = delete;
	x11_idle_detector( x11_idle_detector&&)      = delete;
	x11_idle_detector& operator=( const x11_idle_detector&) = delete;
	x11_idle_detector& operator=( x11_idle_detector&& ) = delete;

	seconds time_idle() override;
	bool idle()         override;
private:
	void   poll_status();
	seconds  idle_seconds = 0s;
	time_point<system_clock>  last_poll { system_clock::from_time_t(0) };
	time_point<system_clock>   last_activity;
	seconds    idle_timeout{2000}; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	bool     is_idle{false};
};

}
#endif /* X11_IDLE_DETECTOR_H */
