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
#include <vector>

namespace
{
const int secsPerMinute=60;
}

using std::vector;


IdleDetector::IdleDetector(const boost::shared_ptr<Timer>& timer)
{
	m_timer = timer;
	timeAdjuster = time(NULL) - getTimestamp();
	idleSeconds = 0;
	m_timer->attach(this);
}

IdleDetector::~IdleDetector()
{
	m_timer->detach(this);
}
void IdleDetector::on_signal_10_seconds()
{
	pollStatus();
}

long IdleDetector::getTimestamp()
{
	return x11property.get_cardinal( "_NET_WM_USER_TIME",0);
}
void IdleDetector::pollStatus()
{
    long now=time(NULL);
    long lastAction = timeAdjuster + getTimestamp();
	idleSeconds = now-lastAction;
}

int IdleDetector::minutesIdle()
{
	return idleSeconds/ secsPerMinute;
}

time_t IdleDetector::timeIdle()
{
	return idleSeconds;
}
