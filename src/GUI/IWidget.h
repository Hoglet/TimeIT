/*
 * IWidget.h
 *
 *  Created on: Mar 23, 2009
 *      Author: hoglet
 */

#ifndef IWIDGET_H_
#define IWIDGET_H_

#include <IActionObserver.h>

class IWidget
{
public:
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void attach(IActionObserver*) {};
	virtual void detach(IActionObserver*) {};
};

#endif /* IWIDGET_H_ */
