#ifndef X11_INFO_H
#define X11_INFO_H

#if XSCREENSAVER_FOUND
#if X11_FOUND
#include <X11/extensions/scrnsaver.h>


namespace libtimeit
{

class x11_info
{
public:
	x11_info();
	~x11_info();
	x11_info(const x11_info&) = delete;
	x11_info& operator=(const x11_info&) = delete;

	[[nodiscard]] XScreenSaverInfo* get() const;
	operator XScreenSaverInfo *() const;
	XScreenSaverInfo *operator ->() const;

private:
	XScreenSaverInfo* x_info = nullptr;
};

} // libtimeit
#endif //X11_FOUND
#endif	//XSCREENSAVER_FOUND
#endif //X11_INFO_H
