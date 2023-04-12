#ifndef X11_INFO_H
#define X11_INFO_H

#if XSCREENSAVER_FOUND
#include <X11/extensions/scrnsaver.h>


namespace libtimeit
{

class x11_info
{
public:
	x11_info();
	~x11_info();
	operator XScreenSaverInfo *() const;
	XScreenSaverInfo *operator ->() const;

private:
	XScreenSaverInfo* x_info = nullptr;
};

} // libtimeit
#endif	//XSCREENSAVER_FOUND
#endif //X11_INFO_H
