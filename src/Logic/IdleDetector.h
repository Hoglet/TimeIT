/*
 * IdleDetector.h
 *
 *  Created on: 2008-aug-27
 *      Author: hoglet
 */

#ifndef IDLEDETECTOR_H_
#define IDLEDETECTOR_H_
#include <Timer.h>
#include <time.h>
#include <memory>

class IIdleDetector
{
public:
	virtual ~IIdleDetector();
	virtual int minutesIdle() = 0;
	virtual time_t timeIdle() = 0;
};


class IdleDetector : public TimerObserver, public IIdleDetector
{
public:
	IdleDetector();
	virtual ~IdleDetector();
	int minutesIdle();
	time_t timeIdle();
	void setIdleTimeout(int minutes);
	void reset();
	bool idle();
	void setEnabled(bool state);
private:
	void pollStatus();
	bool IdleDetectionPossible;
	long getTimestamp();
	time_t idleSeconds;
	time_t lastPoll;
	long idleTimeout;
	bool isIdle;
	time_t lastActivity;
	bool enabled;
};

#endif /* IDLEDETECTOR_H_ */
