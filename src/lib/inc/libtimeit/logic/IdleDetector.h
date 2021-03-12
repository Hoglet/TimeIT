/*
 * IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef IDLEDETECTOR_H_
#define IDLEDETECTOR_H_
#include <libtimeit/Timer.h>
#include <time.h>
#include <memory>

class IIdleDetector
{
public:
	virtual ~IIdleDetector();
	virtual int minutesIdle() = 0;
	virtual time_t timeIdle() = 0;
};


class IdleDetector : public libtimeit::TimerObserver, public IIdleDetector
{
public:
	IdleDetector();
	virtual ~IdleDetector();
	int minutesIdle();
	time_t timeIdle();
	void setIdleTimeout(int minutes);
	void reset();
	bool idle();
private:
	void pollStatus();
	bool IdleDetectionPossible;
	time_t idleSeconds;
	time_t lastPoll = 0;
	long idleTimeout;
	bool isIdle;
	time_t lastActivity;
};

#endif /* IDLEDETECTOR_H_ */
