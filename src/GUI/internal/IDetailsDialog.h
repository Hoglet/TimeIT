/*
 * IDetailsDialog.h
 *
 *  Created on: Jan 17, 2009
 *      Author: hoglet
 */

#ifndef IDETAILSDIALOG_H_
#define IDETAILSDIALOG_H_

#include "ISummary.h"
namespace GUI
{
class IDetailsDialog
{
public:
	virtual ~IDetailsDialog() {};
	virtual void setTimeEntryID(int64_t id) = 0;
	virtual void set(int64_t ID,time_t startTime,time_t stopTime) = 0;
	virtual void  show() = 0;
};

}
#endif /* IDETAILSDIALOG_H_ */
