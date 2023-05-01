#if X11_FOUND

#include "x_lib_accessor.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace libtimeit
{


//LCOV_EXCL_START
x_lib_accessor::x_lib_accessor()
{
	root_window = DefaultRootWindow(display.get());
}

bool x_lib_accessor::is_open( )
{
	return display != nullptr;
}

x_lib_accessor::~x_lib_accessor()
{
}

int x_lib_accessor::viewport_width()
{
	Screen *screen = DefaultScreenOfDisplay(display.get());
	return screen->width;
}

int x_lib_accessor::viewport_height()
{
	Screen *screen = DefaultScreenOfDisplay(display.get());
	return screen->height;
}


long x_lib_accessor::get_cardinal(const char *name, int offset) noexcept(false)
{
	Atom property_name = XInternAtom(display, name, False);
	Atom property_type = XA_CARDINAL;

	Atom returned_type = 0;
	int  returned_format = 0;
	unsigned long number_of_items = 0;
	unsigned long bytes_after_return = 0;
	unsigned char *returned_data = nullptr;

	long return_value;  // NOLINT

	if (XGetWindowProperty(
			display, root_window, property_name, offset, 1, False, property_type, &returned_type,
			&returned_format, &number_of_items, &bytes_after_return, &returned_data) == Success)
	{
		if (returned_format == 32 && returned_type == property_type && number_of_items > 0) // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		{
			return_value = ((long *) returned_data)[0]; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic,cppcoreguidelines-pro-type-cstyle-cast)
			XFree(returned_data);
		}
		else
		{
			XFree(returned_data);
			throw general_exception("get_cardinal failed: Unexpected data");

		}
	}
	else
	{
		throw general_exception("get_cardinal failed: XGetWindowProperty failed");
	}
	return return_value;
}

vector<string> x_lib_accessor::get_strings(const char *name) noexcept(false)
{
	Atom property_name = XInternAtom(display, name, False);
	Atom property_type = XInternAtom(display, "UTF8_STRING", False);

	Atom returned_type = 0;
	int  returned_format = 0;
	unsigned long  number_of_items = 0;
	unsigned long  bytes_after_return = 0;
	unsigned char* returned_data=nullptr;
	vector<string> return_values;

	if (XGetWindowProperty(
			display, root_window, property_name, 0, 1024, False, property_type, &returned_type, // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
			&returned_format, &number_of_items, &bytes_after_return, &returned_data) == Success) // NOLINT(cppcoreguidelines-avoid-magic-numbers)
	{

		if (returned_type == property_type && number_of_items > 0)
		{
			unsigned int pos = 0;
			while (pos < number_of_items)
			{
				string str = (char *) (&returned_data[pos]); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast,cppcoreguidelines-pro-bounds-pointer-arithmetic)
				return_values.push_back(str);
				pos += str.length() + 1;
			}
			XFree(returned_data);
		}
		else
		{
			XFree(returned_data);
			throw general_exception("get_strings failed: Unexpected data");
		}
	}
	else
	{
		throw general_exception("get_cardinal failed: XGetWindowProperty failed");
	}
	return return_values;
}

//LCOV_EXCL_STOP
}

#endif	// X11_FOUND
