/*
 * X11_IdleDetector.cpp
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#include "libtimeit/logic/X11_idle_detector.h"
#include <iostream>
#include <libtimeit/timer.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <memory>
#include <libtimeit/utils.h>
#include <exception>

namespace libtimeit
{
const int secsPerMinute = 60;
Display* display = 0;
XScreenSaverInfo* XInfo = 0;

using namespace std;

bool X11_idle_detector::available()
{
	int event_base, error_base;
	auto dsp = XOpenDisplay(0);

	if (XScreenSaverQueryExtension(dsp, &event_base, &error_base))
	{
		return true;
	}
	else
	{
		return false;
	}

}

X11_idle_detector::X11_idle_detector(Timer &timer) : Timer_observer(timer)
{
	if (!available())
	{
		cerr << "Unable to detect if user is idle. XScreenSaverQueryExtension not available\n";
		throw runtime_error("X11 exception");
	}
	display = XOpenDisplay(0);
	XInfo = XScreenSaverAllocInfo();
	idle_seconds = 0;
	last_poll = libtimeit::now();
	idle_timeout_ = 2000;
	last_activity = last_poll;
	is_idle = false;
}

X11_idle_detector::~X11_idle_detector()
{
	if (XInfo)
	{
		XFree(XInfo);
	}
	if (display)
	{
		XCloseDisplay(display);
	}
}

void X11_idle_detector::idle_timeout(unsigned minutes)
{
	idle_timeout_ = minutes * 60;
}

void X11_idle_detector::reset()
{
	is_idle = false;
	last_poll = libtimeit::now();
}

void X11_idle_detector::poll_status()
{
	time_t now = libtimeit::now();
	auto pollTime = now - last_poll;
	last_poll = now;

	// Check if we have been suspended
	if (pollTime > idle_timeout_)
	{
		//We have been suspended
		idle_seconds = now - last_activity;
		is_idle = true;
		return;
	}

	XScreenSaverQueryInfo(display, XRootWindow(display, 0), XInfo);
	idle_seconds = (XInfo->idle / 1000);

	if (idle_seconds < 20)
	{
		last_activity = now;
	}
	if (idle_seconds > idle_timeout_)
	{
		is_idle = true;
	}
	else
	{
		is_idle = false;
	}

}

bool X11_idle_detector::idle()
{
	poll_status();
	return is_idle;
}

int X11_idle_detector::minutes_idle()
{
	return time_idle() / secsPerMinute;
}

time_t X11_idle_detector::time_idle()
{
	poll_status();
	return idle_seconds;
}

}