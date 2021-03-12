/*
 * IdleDetector.cpp
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#include "libtimeit/logic/IdleDetector.h"
#include <iostream>
#include <libtimeit/Timer.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <memory>
#include <libtimeit/Utils.h>


namespace
{
const int secsPerMinute = 60;
Display* display = 0;
XScreenSaverInfo* XInfo = 0;
}

IIdleDetector::~IIdleDetector()
{

}

IdleDetector::IdleDetector()
{
	int event_base, error_base;
	if (display == 0)
	{
		display = XOpenDisplay(0);
	}
	if (XInfo == 0)
	{
		XInfo = XScreenSaverAllocInfo();
	}
	IdleDetectionPossible = false;
	if (XScreenSaverQueryExtension(display, &event_base, &error_base))
	{
		IdleDetectionPossible = true;
	}
	idleSeconds = 0;
	lastPoll = libtimeit::now();
	idleTimeout = 2000;
	lastActivity = lastPoll;
	isIdle = false;
}

IdleDetector::~IdleDetector()
{
	if (XInfo)
	{
		//TODO safer allocation/dealocation!
		XFree(XInfo);
    XInfo = nullptr;
	}
}
void IdleDetector::setIdleTimeout(int minutes)
{
	idleTimeout = minutes * 60;
}

void IdleDetector::reset()
{
	isIdle = false;
	lastPoll = libtimeit::now();
}

void IdleDetector::pollStatus()
{
	if (!IdleDetectionPossible)
	{
		return;
	}
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

bool IdleDetector::idle()
{
	pollStatus();
	return isIdle;
}

int IdleDetector::minutesIdle()
{
	return timeIdle() / secsPerMinute;
}

time_t IdleDetector::timeIdle()
{
	pollStatus();
	return idleSeconds;
}

