#include "MockStatusIcon.h"

namespace Test
{

MockStatusIcon::MockStatusIcon()
{
	// TODO Auto-generated constructor stub
	visible = false;
}

MockStatusIcon::~MockStatusIcon()
{
	// TODO Auto-generated destructor stub
}


void MockStatusIcon::show()
{
	visible = true;
}

void MockStatusIcon::attach(IActionObserver* observer)
{

}

void MockStatusIcon::detach(IActionObserver* observer)
{

}


} /* namespace Test */
