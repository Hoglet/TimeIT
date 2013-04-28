/*
 * ISummary.h
 *
 *  Created on: Jan 18, 2009
 *      Author: hoglet
 */

#ifndef ISUMMARY_H_
#define ISUMMARY_H_

#include <list>
#include <stdint.h>
#include <time.h>
namespace GUI
{

class ISummary;
class SummaryObserver
{
public:
	SummaryObserver();
	virtual ~SummaryObserver();
	virtual void on_selection_changed(int64_t id, time_t startTime, time_t stopTime) = 0;
	virtual void on_showDetailsClicked(ISummary* summary,int64_t taskId, time_t startTime, time_t stopTime) = 0;
	void attach(ISummary* subject);
	void detach(ISummary* subject);
private:
	bool unsubscription_allowed;
	std::list<ISummary* > subjects;
};
class ISummary
{
public:
	virtual ~ISummary() {};
	virtual void attach(SummaryObserver*) = 0;
	virtual void detach(SummaryObserver*) = 0;
};

}
#endif /* ISUMMARY_H_ */
