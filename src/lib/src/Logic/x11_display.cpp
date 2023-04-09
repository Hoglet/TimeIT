#include <X11/Xlib.h>
#include <exception>
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

} // libtimeit
