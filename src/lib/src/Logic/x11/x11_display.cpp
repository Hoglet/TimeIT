#if X11_FOUND

#include <X11/Xlib.h>
#include <stdexcept>
#include "x11_display.h"

namespace libtimeit
{
using namespace std;

x11_display::x11_display( )
{
	display = XOpenDisplay( nullptr );
	if ( display == nullptr )
	{
		throw domain_error( "Unable to connect to X11" );
	}
}

x11_display::~x11_display( )
{
	XCloseDisplay( display );
}

x11_display::operator Display*( ) const
{
	return display;
}

Display* x11_display::get( ) const
{
	return display;
}
} // libtimeit
#endif //X11_FOUND
