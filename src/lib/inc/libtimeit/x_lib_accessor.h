#ifndef X_LIB_ACCESSOR_H
#define X_LIB_ACCESSOR_H
#include <vector>
#include <string>
#include <X11/Xlib.h>
#include <libtimeit/exceptions/general_exception.h>
namespace libtimeit
{
using namespace std;

class x_lib_accessor
{
public:
	x_lib_accessor();
	x_lib_accessor(const x_lib_accessor&) = delete;
	x_lib_accessor(x_lib_accessor&&) = delete;
	x_lib_accessor& operator=(x_lib_accessor&&) = delete;
	x_lib_accessor& operator=(const x_lib_accessor&) = delete;

	virtual ~x_lib_accessor();
	long            get_cardinal(const char *prop_name, int index) noexcept(false);
	vector<string>  get_strings(const char *prop_name)             noexcept(false);
	int             viewport_width();
	int             viewport_height();
	bool is_open( );
private:
	Display          *display;
	Window            root_window;
};

}

#endif /* X_LIB_ACCESSOR_H */
