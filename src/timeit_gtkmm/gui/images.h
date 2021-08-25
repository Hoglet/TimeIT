#ifndef IMAGES_H
#define IMAGES_H

#include <string>
#include <gtkmm.h>
#include <glibmm/refptr.h>

namespace gui
{
using namespace std;
using namespace Gdk;

enum class image_identifier
{
	STD_ICON,
	RUNNING,
	RUNNING_IDLE,
	BLANK,
	DEFAULT,
	RUNNING_SMALL,
	BLANK_SMALL,
	RUNNING_BIG
};

using image_pointer = Glib::RefPtr<Pixbuf>;

class image_cache
{
public:
	image_cache();
	image_pointer by_id(image_identifier id);
private:
	void create_image(image_identifier id, int width, int height);

	const string image_path    = PROGRAMNAME_DATADIR;
	const string image_src_dir = IMAGE_SRC_DIR;

	map<image_identifier, image_pointer> images{};
};

}
#endif //IMAGES_H
