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

class IdleDetector : public TimerObserver
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
	long getTimestamp();
	time_t idleSeconds;
	time_t lastPoll;
	long idleTimeout;
	bool isIdle;
	time_t lastActivity;

};

#endif /* IDLEDETECTOR_H_ */
