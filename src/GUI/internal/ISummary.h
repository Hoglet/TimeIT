/*
 * ISummary.h
 *
 *  Created on: Jan 18, 2009
 *      Author: hoglet
 */

#ifndef ISUMMARY_H_
#define ISUMMARY_H_

#include "boost/weak_ptr.hpp"
#include <list>
namespace GUI
{

class ISummary;
class SummaryObserver
{
public:
	SummaryObserver();
	~SummaryObserver();
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
	virtual void attach(SummaryObserver*) = 0;
	virtual void detach(SummaryObserver*) = 0;
};

}
#endif /* ISUMMARY_H_ */
