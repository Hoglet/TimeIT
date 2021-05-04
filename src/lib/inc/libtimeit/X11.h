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
#include <libtimeit/exceptions/general_exception.h>
namespace libtimeit
{
using namespace std;

class X11
{
public:
	X11();
	X11(const X11&) = delete;
	X11(X11&&) = delete;
	X11& operator=(X11&&) = delete;
	X11& operator=(const X11&) = delete;

	virtual ~X11();
	long            get_cardinal(const char *prop_name, int index) noexcept(false);
	vector<string>  get_strings(const char *prop_name)             noexcept(false);
	int             viewport_width();
	int             viewport_height();
private:
	Display          *display;
	Window            root_window;
};

}

#endif /* X11PROPERTY_H_ */
