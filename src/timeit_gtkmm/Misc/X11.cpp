/*
 * X11Property.cpp
 *
 *  Created on: May 10, 2010
 *      Author: hoglet
 */

#include "X11.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

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

int X11::getViewportWidth()
{
	Screen* screen = DefaultScreenOfDisplay(display);
	return screen->width;
}

int X11::getViewportHeight()
{
	Screen* screen = DefaultScreenOfDisplay(display);
	return screen->height;
}


long X11::getCardinal(const char *name, int offset) noexcept(false)
{
	Atom propertyName = XInternAtom(display, name, False);
	Atom propertyType = XA_CARDINAL;

	Atom returnedType;
	int returnedFormat;
	unsigned long numberOfItems;
	unsigned long bytes_after_return;
	unsigned char *returnedData;

	long returnValue = 0;

	if (XGetWindowProperty(display, rootWindow, propertyName, offset, 1, False, propertyType, &returnedType,
			&returnedFormat, &numberOfItems, &bytes_after_return, &returnedData) == Success)
	{
		if (returnedFormat == 32 && returnedType == propertyType && numberOfItems > 0)
		{
			returnValue = ((long*) returnedData)[0];
			XFree(returnedData);
		}
		else
		{
			XFree(returnedData);
			e.setMessage("get_cardinal failed: Unexpected data");
			throw e;
		}
	}
	else
	{
		e.setMessage("get_cardinal failed: XGetWindowProperty failed");
		throw e;
	}
	return returnValue;
}

std::vector<std::string> X11::getStrings(const char *name) noexcept(false)
{
	Atom propertyName = XInternAtom(display, name, False);
	Atom propertyType = XInternAtom(display, "UTF8_STRING", False);

	Atom returnedType;
	int returnedFormat;
	unsigned long numberOfItems;
	unsigned long bytes_after_return;
	unsigned char *returnedData;
	std::vector<std::string> returnValues;

	if (XGetWindowProperty(display, rootWindow, propertyName, 0, 1024, False, propertyType, &returnedType,
			&returnedFormat, &numberOfItems, &bytes_after_return, &returnedData) == Success)
	{

		if (returnedType == propertyType && numberOfItems > 0)
		{
			unsigned int pos = 0;
			while (pos < numberOfItems)
			{
				std::string str = (char*) (&returnedData[pos]);
				returnValues.push_back(str);
				pos += str.length() + 1;
			}
			XFree(returnedData);
		}
		else
		{
			XFree(returnedData);
			e.setMessage("get_strings failed: Unexpected data");
			throw e;
		}
	}
	else
	{
		e.setMessage("get_cardinal failed: XGetWindowProperty failed");
		throw e;
	}
	return returnValues;
}
//LCOV_EXCL_STOP
