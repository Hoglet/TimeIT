/*
 * IWidget.h
 *
 *  Created on: Mar 23, 2009
 *      Author: hoglet
 */

#ifndef IWIDGET_H_
#define IWIDGET_H_

#include <action_observer.h>

class IWidget
{
public:
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void move(int x, int y) = 0;
	virtual bool is_visible() = 0;
	virtual void get_position(int& Window_x, int& Window_y) = 0;

	virtual void attach(action_observer* /*observer*/) {};
	virtual void detach(action_observer* /*observer*/) {};
};

#endif /* IWIDGET_H_ */
