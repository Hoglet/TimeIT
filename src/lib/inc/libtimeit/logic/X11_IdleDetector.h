/*
 * X11_IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef IDLEDETECTOR_H_
#define IDLEDETECTOR_H_
#include <libtimeit/Timer.h>
#include <time.h>
#include <memory>
#include <libtimeit/db/notifier.h>

class IIdleDetector
{
public:
	virtual ~IIdleDetector();
	virtual int minutesIdle() = 0;
	virtual time_t timeIdle() = 0;
};


class X11_IdleDetector : public libtimeit::TimerObserver, public IIdleDetector
{
public:
	static bool available();
	X11_IdleDetector(libtimeit::Timer& timer);
	virtual ~X11_IdleDetector();
	int minutesIdle();
	time_t timeIdle();
	void setIdleTimeout(int minutes);
	void reset();
	bool idle();
private:
	void pollStatus();
	time_t idleSeconds;
	time_t lastPoll = 0;
	long idleTimeout;
	bool isIdle;
	time_t lastActivity;
};

#endif /* IDLEDETECTOR_H_ */
