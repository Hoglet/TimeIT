

#include "x11_info.h"
#if XSCREENSAVER_FOUND
namespace libtimeit
{

x11_info::x11_info()
{
	x_info = XScreenSaverAllocInfo();
}

x11_info::~x11_info()
{
	if( x_info != nullptr)
	{
		XFree( x_info );
	}
}

x11_info::operator XScreenSaverInfo *() const
{
	return x_info;
}

XScreenSaverInfo *x11_info::operator ->() const
{
	return x_info;
}

} // libtimeit
#endif
