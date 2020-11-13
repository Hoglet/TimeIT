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
#include <Misc/Utils.h>

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
	lastPoll = Utils::now();
	idleTimeout = 2000;
	lastActivity = lastPoll;
	isIdle = false;
	enabled = false;
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
	lastPoll = Utils::now();
}

void IdleDetector::pollStatus()
{
	if (IdleDetectionPossible)
	{
		time_t now = Utils::now();
		if (isIdle)
		{
			idleSeconds = now - lastActivity;
		}
		else
		{
			if (now - lastPoll > 30)
			{
				idleSeconds = now - lastActivity;
			}
			else
			{
				XScreenSaverQueryInfo(display, XRootWindow(display, 0), XInfo);
				idleSeconds = (XInfo->idle / 1000);
				lastPoll = now;
				if (idleSeconds < 20)
				{
					lastActivity = now;
				}
				time_t executionTime = Utils::now() -now;
				if( executionTime > 10)
				{
					//We have been suspended for more than 10 seconds inside this function
					idleSeconds = executionTime;
				}
			}
			lastPoll = now;
		}
		if (enabled && idleSeconds > idleTimeout)
		{
		  isIdle = true;
		}
	}
}

bool IdleDetector::idle()
{
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

void IdleDetector::setEnabled(bool state)
{
	enabled = state;
}
