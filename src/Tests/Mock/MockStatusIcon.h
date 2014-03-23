#ifndef MOCKSTATUSICON_H_
#define MOCKSTATUSICON_H_

#include "IStatusIcon.h"

namespace Test
{

class MockStatusIcon: public GUI::IStatusIcon
{
public:
	MockStatusIcon();
	virtual ~MockStatusIcon();

	virtual void show() ;
	virtual void attach(IActionObserver* observer) ;
	virtual void detach(IActionObserver* observer) ;
	bool visible;
};

} /* namespace Test */
#endif /* MOCKSTATUSICON_H_ */
