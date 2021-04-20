/*
 * X11Property.cpp
 *
 *  Created on: May 10, 2010
 *      Author: hoglet
 */

#include "libtimeit/X11.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

namespace libtimeit
{


//LCOV_EXCL_START
X11::X11()
{
	display = XOpenDisplay(NULL);
	rootWindow = DefaultRootWindow(display);

}

X11::~X11()
{
	XCloseDisplay(display);
}

int X11::viewport_width()
{
	Screen *screen = DefaultScreenOfDisplay(display);
	return screen->width;
}

int X11::viewport_height()
{
	Screen *screen = DefaultScreenOfDisplay(display);
	return screen->height;
}


long X11::get_cardinal(const char *name, int offset) noexcept(false)
{
	Atom property_name = XInternAtom(display, name, False);
	Atom property_type = XA_CARDINAL;

	Atom returned_type;
	int  returned_Format;
	unsigned long number_of_items;
	unsigned long bytes_after_return;
	unsigned char *returned_data;

	long return_value;

	if (XGetWindowProperty(
			display, rootWindow, property_name, offset, 1, False, property_type, &returned_type,
			&returned_Format, &number_of_items, &bytes_after_return, &returned_data) == Success)
	{
		if (returned_Format == 32 && returned_type == property_type && number_of_items > 0)
		{
			return_value = ((long *) returned_data)[0];
			XFree(returned_data);
		}
		else
		{
			XFree(returned_data);
			e.setMessage("get_cardinal failed: Unexpected data");
			throw e;
		}
	}
	else
	{
		e.setMessage("get_cardinal failed: XGetWindowProperty failed");
		throw e;
	}
	return return_value;
}

vector<string> X11::get_strings(const char *name) noexcept(false)
{
	Atom property_name = XInternAtom(display, name, False);
	Atom property_type = XInternAtom(display, "UTF8_STRING", False);

	Atom returned_type;
	int  returned_format;
	unsigned long  number_of_items;
	unsigned long  bytes_after_return;
	unsigned char* returned_data;
	vector<string> return_values;

	if (XGetWindowProperty(
			display, rootWindow, property_name, 0, 1024, False, property_type, &returned_type,
			&returned_format, &number_of_items, &bytes_after_return, &returned_data) == Success)
	{

		if (returned_type == property_type && number_of_items > 0)
		{
			unsigned int pos = 0;
			while (pos < number_of_items)
			{
				string str = (char *) (&returned_data[pos]);
				return_values.push_back(str);
				pos += str.length() + 1;
			}
			XFree(returned_data);
		}
		else
		{
			XFree(returned_data);
			e.setMessage("get_strings failed: Unexpected data");
			throw e;
		}
	}
	else
	{
		e.setMessage("get_cardinal failed: XGetWindowProperty failed");
		throw e;
	}
	return return_values;
}
//LCOV_EXCL_STOP
}