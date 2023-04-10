#ifndef X11_DISPLAY_H
#define X11_DISPLAY_H

#include <X11/Xlib.h>

namespace libtimeit
{
class x11_display
{
public:
	x11_display();
	~x11_display();
	operator Display *() const;
private:
	Display* display;
};

} // libtimeit


#endif //X11_DISPLAY_H
