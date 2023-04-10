#ifndef X11_INFO_H
#define X11_INFO_H

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
	XScreenSaverInfo* x_info;
};

} // libtimeit

#endif //X11_INFO_H
