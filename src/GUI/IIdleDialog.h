/*
 * IIdleDialog.h
 *
 *  Created on: Mar 21, 2009
 *      Author: hoglet
 */

#ifndef IIDLEDIALOG_H_
#define IIDLEDIALOG_H_

#include <time.h>
#include <IActionObserver.h>
#include <memory>
#include <vector>
#include <ExtendedTask.h>

class IIdleDialog
{
public:
	virtual ~IIdleDialog() {};
	virtual void setIdleStartTime(time_t idleStartTime) = 0;
	virtual void setActiveTaskList(std::shared_ptr<std::vector<DB::ExtendedTask>> activeTasks) = 0;
	virtual void show() = 0;
	virtual void attach(IActionObserver*) = 0;
	virtual void detach(IActionObserver*) = 0;
};


#endif /* IIDLEDIALOG_H_ */
