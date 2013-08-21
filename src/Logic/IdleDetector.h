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
private:
	void pollStatus();
	bool IdleDetectionPossible;
	long getTimestamp();
	time_t idleSeconds;
	std::shared_ptr<Timer> m_timer;
};

#endif /* IDLEDETECTOR_H_ */
