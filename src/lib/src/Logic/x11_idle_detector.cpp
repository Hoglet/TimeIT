#include <X11/Xlib.h>
#include <X11/extensions/scrnsaver.h>
#include <libtimeit/logic/x_11_idle_detector.h>
#include <iostream>
#include <libtimeit/utils.h>

namespace libtimeit
{
Display*         display = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
XScreenSaverInfo* x_info = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

using namespace std;

bool x_11_idle_detector::available()
{
	int event_base = 0;
	int error_base = 0;
	auto* dsp = XOpenDisplay(nullptr);
	if (!dsp) {
		return false;
	}

	return (XScreenSaverQueryExtension(dsp, &event_base, &error_base)>0);
}

x_11_idle_detector::x_11_idle_detector( timer_base &timer) : timer_observer( timer)
{
	if (!available())
	{
		cerr << "Unable to detect if user is idle. XScreenSaverQueryExtension not available\n";
	}
	display = XOpenDisplay(nullptr);
	x_info = XScreenSaverAllocInfo();
	idle_seconds = 0s;
	last_poll = system_clock::now();
	last_activity = last_poll;
	is_idle = false;
}

x_11_idle_detector::~x_11_idle_detector()
{
	if (x_info != nullptr)
	{
		XFree(x_info);
	}
	if (display != nullptr)
	{
		XCloseDisplay(display);
	}
}



void x_11_idle_detector::poll_status()
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

	if (!display) {
		return;
	}
	XScreenSaverQueryInfo(display, XRootWindow(display, 0), x_info);
	idle_seconds = duration_cast<seconds>(milliseconds(x_info->idle ));

	if (idle_seconds < 20s)
	{
		last_activity = now;
	}
	is_idle = (idle_seconds > idle_timeout);

}

bool x_11_idle_detector::idle()
{
	poll_status();
	return is_idle;
}

seconds x_11_idle_detector::time_idle()
{
	poll_status();
	return idle_seconds;
}

}
