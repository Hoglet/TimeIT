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

class x11_lib
{
public:
	x11_lib();
	x11_lib(const x11_lib&) = delete;
	x11_lib(x11_lib&&) = delete;
	x11_lib& operator=(x11_lib&&) = delete;
	x11_lib& operator=(const x11_lib&) = delete;

	virtual ~x11_lib();
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
