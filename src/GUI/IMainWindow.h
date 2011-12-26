/*
 * IMainWindow.h
 *
 *  Created on: Apr 13, 2009
 *      Author: hoglet
 */

#ifndef IMAINWINDOW_H_
#define IMAINWINDOW_H_
#include <ICalendar.h>

class IMainWindow
{
public:
	virtual ~IMainWindow() {};
	virtual GUI::ICalendar& getCalendar() = 0;
};
#endif /* IMAINWINDOW_H_ */
