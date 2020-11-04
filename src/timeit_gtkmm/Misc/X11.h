/*
 * X11Property.h
 *
 *  Created on: May 10, 2010
 *      Author: hoglet
 */

#ifndef X11PROPERTY_H_
#define X11_H_
#include <vector>
#include <string>
#include <X11/Xlib.h>
#include <GeneralException.h>

class X11
{
public:
	X11();
	virtual ~X11();
	long getCardinal ( const char *prop_name, int index) noexcept(false);
	std::vector<std::string> getStrings ( const char *prop_name) noexcept(false);
	int getViewportWidth();
	int getViewportHeight();
private:
	Display* display;
	Window rootWindow;
	GeneralException e;
};

#endif /* X11PROPERTY_H_ */
