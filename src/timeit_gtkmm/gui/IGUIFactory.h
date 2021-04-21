/*
 * IGUIFactory.h
 *
 *  Created on: Jan 18, 2009
 *      Author: hoglet
 */

#ifndef IGUIFACTORY_H_
#define IGUIFACTORY_H_

#include "IStatusIcon.h"
#include "ICalendar.h"
#include <IWidget.h>
#include <memory>
#include <stdint.h>

namespace GUI
{

enum EWidget
{
	MAIN_WINDOW,
	ADD_TASK_DIALOG,
	ABOUT_DIALOG,
	EDIT_TASK_DIALOG,
	ADD_TIME,
	IDLE_DIALOG,
	DETAILS_DIALOG,
	PREFERENCE_DIALOG,
	MAX_WIDGETS
};

typedef std::shared_ptr<IWidget> WidgetPtr;

class IGUIFactory
{
public:
	virtual ~IGUIFactory() {};
	virtual WidgetPtr getWidget(EWidget) = 0;
	virtual IStatusIcon& getStatusIcon() = 0;
	virtual WidgetPtr getAddTime(int64_t taskID) = 0;
	virtual void quit() = 0;
};
}
#endif /* IGUIFACTORY_H_ */
