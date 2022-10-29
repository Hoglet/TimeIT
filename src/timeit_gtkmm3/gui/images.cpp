#include "images.h"
#include <libtimeit/os_abstraction.h>

namespace gui
{
using namespace Glib;
using namespace Gdk;
using namespace libtimeit;

const map<image_identifier, string> IMAGE_FILE_NAMES{
		{image_identifier::STD_ICON,     "icon.svg"},
		{image_identifier::RUNNING,      "running.svg"},
		{image_identifier::RUNNING_IDLE, "running-idle.svg" },
		{image_identifier::BLANK,        "blank.svg"},
		{image_identifier::DEFAULT,      "icon.svg"},
		{image_identifier::RUNNING_SMALL,"running.svg"},
		{image_identifier::BLANK_SMALL,  "blank.svg"},
		{image_identifier::RUNNING_BIG,  "running.svg"},

};

image_cache::image_cache()
{
	create_image(image_identifier::STD_ICON,200,200);
	create_image(image_identifier::RUNNING,24,24);
	create_image(image_identifier::RUNNING_IDLE,24,24);
	create_image(image_identifier::BLANK,24,24);
	create_image(image_identifier::DEFAULT, 24,24);
	create_image(image_identifier::RUNNING_SMALL, 15,15);
	create_image(image_identifier::BLANK_SMALL, 15, 15);
	create_image(image_identifier::RUNNING_BIG,200,200);
}

image_pointer image_cache::by_id(image_identifier id)
{
	if(images.find(id)!=images.end())
	{
		return images.at(id);
	}
	return images.at(image_identifier::DEFAULT);
}

void image_cache::create_image(image_identifier id, int width, int height)
{
	auto file_name=IMAGE_FILE_NAMES.at( id);
	string icon_path = build_filename(image_path, file_name );
	if( ! file_exists(icon_path) )
	{
		icon_path = build_filename(image_src_dir, file_name );
	}
	if( file_exists(icon_path) )
	{
		auto image = Pixbuf::create_from_file(icon_path, width, height, true);
		images.insert_or_assign(id, image);
	}
}


}