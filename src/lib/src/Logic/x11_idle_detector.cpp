/*
 * X11_IdleDetector.cpp
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */
#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <libtimeit/logic/x11_idle_detector.h>
#include <iostream>
#include <libtimeit/timer.h>
#include <memory>
#include <libtimeit/utils.h>

namespace libtimeit
{
constexpr const auto MINUTE=60;
Display*         display = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
XScreenSaverInfo* x_info = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

using namespace std;

bool X11_idle_detector::available()
{
	int event_base = 0;
	int error_base = 0;
	auto* dsp = XOpenDisplay(nullptr);

	return (XScreenSaverQueryExtension(dsp, &event_base, &error_base)>0);
}

X11_idle_detector::X11_idle_detector(Timer &timer) : timer_observer(timer)
{
	if (!available())
	{
		cerr << "Unable to detect if user is idle. XScreenSaverQueryExtension not available\n";
		throw runtime_error("X11 exception");
	}
	display = XOpenDisplay(nullptr);
	x_info = XScreenSaverAllocInfo();
	idle_seconds = 0;
	last_poll = libtimeit::now();
	last_activity = last_poll;
	is_idle = false;
}

X11_idle_detector::~X11_idle_detector()
{
	if (x_info != nullptr)
	{
		XFree(x_info);
	}
	if (display != nullptr)
	{
		XCloseDisplay(display);
	}
}



void X11_idle_detector::poll_status()
{
	time_t now = libtimeit::now();
	auto poll_time = now - last_poll;
	last_poll = now;

	// Check if we have been suspended
	if (poll_time > MINUTE)
	{
		//We have been suspended
		idle_seconds = now - last_activity;
		is_idle = true;
		return;
	}

	XScreenSaverQueryInfo(display, XRootWindow(display, 0), x_info);
	idle_seconds = (time_t)(x_info->idle / 1000); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

	if (idle_seconds < 20) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
	{
		last_activity = now;
	}
	is_idle = (idle_seconds > idle_timeout);

}

bool X11_idle_detector::idle()
{
	poll_status();
	return is_idle;
}

time_t X11_idle_detector::time_idle()
{
	poll_status();
	return idle_seconds;
}

}