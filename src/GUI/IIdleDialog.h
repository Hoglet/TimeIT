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

class IIdleDialog
{
public:
	virtual ~IIdleDialog() {};
	virtual void  setIdleStartTime(time_t idleStartTime) = 0;
	virtual void show() = 0;
	virtual void attach(IActionObserver*) = 0;
	virtual void detach(IActionObserver*) = 0;
};


#endif /* IIDLEDIALOG_H_ */
