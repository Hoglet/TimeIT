/*
 * X11_IdleDetector.cpp
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#include "libtimeit/logic/X11_IdleDetector.h"
#include <iostream>
#include <libtimeit/Timer.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <memory>
#include <libtimeit/Utils.h>
#include <exception>


namespace
{
const int secsPerMinute = 60;
Display* display = 0;
XScreenSaverInfo* XInfo = 0;
}

using namespace std;

IIdleDetector::~IIdleDetector()
{

}

bool X11_IdleDetector::available()
{
	int event_base, error_base;
	auto dsp = XOpenDisplay(0);

	if ( XScreenSaverQueryExtension(dsp, &event_base, &error_base))
	{
		return true;
	}
	else
	{
		return false;
	}

}

X11_IdleDetector::X11_IdleDetector()
{
	if(!available())
	{
		cerr << "Unable to detect if user is idle. XScreenSaverQueryExtension not available\n";
		throw runtime_error("X11 exception");
	}
	display = XOpenDisplay(0);
	XInfo = XScreenSaverAllocInfo();
	idleSeconds = 0;
	lastPoll = libtimeit::now();
	idleTimeout = 2000;
	lastActivity = lastPoll;
	isIdle = false;
}

X11_IdleDetector::~X11_IdleDetector()
{
	if (XInfo)
	{
		XFree(XInfo);
	}
	if(display)
	{
		XCloseDisplay(display);
	}
}
void X11_IdleDetector::setIdleTimeout(int minutes)
{
	idleTimeout = minutes * 60;
}

void X11_IdleDetector::reset()
{
	isIdle = false;
	lastPoll = libtimeit::now();
}

void X11_IdleDetector::pollStatus()
{
	time_t now = libtimeit::now();
	auto pollTime = now - lastPoll;
	lastPoll = now;

	// Check if we have been suspended
	if ( pollTime > idleTimeout)
	{
		//We have been suspended
		idleSeconds = now-lastActivity;
		isIdle = true;
		return;
	}

	XScreenSaverQueryInfo(display, XRootWindow(display, 0), XInfo);
	idleSeconds = (XInfo->idle / 1000);

	if (idleSeconds < 20)
	{
		lastActivity = now;
	}
	if  (idleSeconds > idleTimeout)
	{
		isIdle = true;
	}
	else
	{
		isIdle = false;
	}

}

bool X11_IdleDetector::idle()
{
	pollStatus();
	return isIdle;
}

int X11_IdleDetector::minutesIdle()
{
	return timeIdle() / secsPerMinute;
}

time_t X11_IdleDetector::timeIdle()
{
	pollStatus();
	return idleSeconds;
}

