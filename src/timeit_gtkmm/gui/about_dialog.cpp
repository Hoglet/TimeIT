/*
 * AboutDialog.cpp
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#include "about_dialog.h"
#include <libtimeit/os_abstraction.h>
#include <glibmm/i18n.h>
#include <libtimeit/version.h>
namespace gui
{
using namespace libtimeit;

void about_dialog::on_link_clicked(AboutDialog& /*about_dialog*/, const Glib::ustring &link)
{
	show_url( link );
}

about_dialog::about_dialog(image_cache& images)
{
	set_skip_pager_hint(true);
	set_skip_taskbar_hint(true);

	set_name("TimeIT");
	set_version(VERSION);

	set_copyright("Kent Asplund");
	//Tagline
	set_comments(_("The unobtrusive time tracker\n"));
	//This is the licence txt
	set_license(_("This program is licenced under GNU General Public Licence (GPL) version 2."));



	set_logo( images.by_id(image_identifier::STD_ICON));

	set_url_hook(
			[](AboutDialog &dialog, const Glib::ustring& link)
			{
				about_dialog::on_link_clicked(dialog, link);
			});
	set_website("https://hoglet.github.io/TimeIT/");
	set_website_label(_("TimeIT website"));

	std::list < Glib::ustring > list_authors;
	list_authors.emplace_back("Kent Asplund");
	set_copyright("Kent Asplund");
	set_authors (list_authors);
	/* This is where you add your name, so you will be credited as translator */
	set_translator_credits(_("translator-credits"));
}
}
