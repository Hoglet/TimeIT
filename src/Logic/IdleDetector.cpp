/*
 * IdleDetector.cpp
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#include "IdleDetector.h"
#include <iostream>
#include <Timer.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <memory>

namespace
{
const int secsPerMinute = 60;
Display* display = 0;
XScreenSaverInfo* XInfo = 0;
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
}

IdleDetector::~IdleDetector()
{
	if (XInfo)
	{
		//TODO safer allocation/dealocation!
		XFree(XInfo);
		XInfo=nullptr;
	}
}

void IdleDetector::pollStatus()
{
	if (IdleDetectionPossible)
	{
		XScreenSaverQueryInfo(display, XRootWindow(display, 0), XInfo);
		idleSeconds = (XInfo->idle / 1000);
	}
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
