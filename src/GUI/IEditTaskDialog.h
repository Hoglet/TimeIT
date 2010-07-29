/*
 * IEditTaskDialog.h
 *
 *  Created on: Jan 4, 2009
 *      Author: hoglet
 */

#ifndef IEDITTASKDIALOG_H_
#define IEDITTASKDIALOG_H_
#include <stdint.h>

namespace GUI
{
class IEditTaskDialog
{
public:
	virtual void setTaskID(int64_t) = 0;
};
}

#endif /* IEDITTASKDIALOG_H_ */
