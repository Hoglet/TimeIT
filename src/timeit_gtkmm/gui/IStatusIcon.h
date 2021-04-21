/*
 * IStatusIcon.h
 *
 *  Created on: Jan 17, 2009
 *      Author: hoglet
 */

#ifndef ISTATUSICON_H_
#define ISTATUSICON_H_

#include <action_observer.h>

namespace GUI
{

class IStatusIcon
{
public:
	virtual ~IStatusIcon() {};
	virtual void show() = 0;
	virtual void attach(action_observer* observer) = 0;
	virtual void detach(action_observer* observer) = 0;
};

}
#endif /* ISTATUSICON_H_ */
