#if X11_FOUND

#include "x11_idle_detector.h"
#include <iostream>
#include "libtimeit/utils.h"
#include "x11_display.h"
#include "x11_info.h"

#include <X11/extensions/scrnsaver.h>

namespace libtimeit
{

using namespace std;

bool x11_idle_detector::available()
{
	int event_base = 0;
	int error_base = 0;
#if XSCREENSAVER_FOUND
	try
	{
		x11_display display;
		return (XScreenSaverQueryExtension(display, &event_base, &error_base)>0);
	}
	catch( exception& e)  {}
#endif
	return false;

}

x11_idle_detector::x11_idle_detector( )
{
	idle_seconds = 0s;
	last_poll = system_clock::now();
	last_activity = last_poll;
	is_idle = false;
}





void x11_idle_detector::poll_status()
{
	auto now = system_clock::now();
	auto poll_time = now - last_poll;
	last_poll = now;

	// Check if we have been suspended
	if (poll_time > 1min)
	{
		//We have been suspended
		idle_seconds = duration_cast<seconds>(now - last_activity);
		is_idle = true;
		return;
	}
#if XSCREENSAVER_FOUND
	try
	{
		x11_display display;
		x11_info    x_info;
		XScreenSaverQueryInfo( display, XRootWindow( display, 0 ), x_info );
		idle_seconds = duration_cast<seconds>( milliseconds( x_info->idle ));
	}
	catch (exception& e)
#endif
	{
		return;
	}

	if (idle_seconds < 20s)
	{
		last_activity = now;
	}
	is_idle = (idle_seconds > idle_timeout);

}

bool x11_idle_detector::idle()
{
	poll_status();
	return is_idle;
}

seconds x11_idle_detector::time_idle()
{
	poll_status();
	return idle_seconds;
}

}

#endif	//X11_FOUND
