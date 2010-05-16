/*
 * X11Property.h
 *
 *  Created on: May 10, 2010
 *      Author: hoglet
 */

#ifndef X11PROPERTY_H_
#define X11PROPERTY_H_
#include <vector>
#include <string>
#include <X11/Xlib.h>

class X11Property
{
public:
	X11Property();
	virtual ~X11Property();
	long get_cardinal ( const char *prop_name, int index);
	std::vector<std::string> get_strings ( const char *prop_name);
private:
	Display* display;
	Window rootWindow;
};

#endif /* X11PROPERTY_H_ */
