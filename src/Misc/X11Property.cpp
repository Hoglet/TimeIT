/*
 * X11Property.cpp
 *
 *  Created on: May 10, 2010
 *      Author: hoglet
 */

#include "X11Property.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <GeneralException.h>

X11Property::X11Property()
{
	display = XOpenDisplay(NULL);
	rootWindow = DefaultRootWindow(display);

}

X11Property::~X11Property()
{
	XCloseDisplay(display);
}

long X11Property::get_cardinal(const char *name, int offset)
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
			throw new GeneralException("get_cardinal failed: Unexpected data");
		}
	}
	else
	{
		throw new GeneralException("get_cardinal failed: XGetWindowProperty failed");
	}
	return returnValue;
}

std::vector<std::string> X11Property::get_strings(const char *name)
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

		if (returnedType != propertyType && numberOfItems > 0)
		{
			int pos = 0;
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
			throw new GeneralException("get_strings failed: Unexpected data");
		}
	}
	else
	{
		throw new GeneralException("get_cardinal failed: XGetWindowProperty failed");
	}
	return returnValues;
}

