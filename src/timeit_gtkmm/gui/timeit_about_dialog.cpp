/*
 * AboutDialog.cpp
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#include "timeit_about_dialog.h"
#include <libtimeit/OS_abstraction.h>
#include <glibmm/i18n.h>
#include <libtimeit/utils.h>
#include <libtimeit/version.h>
namespace gui
{
using namespace libtimeit;

void TimeItAboutDialog::on_link_clicked(AboutDialog &about_dialog, const Glib::ustring &link)
{
	show_URL(link);
}

TimeItAboutDialog::TimeItAboutDialog()
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

	std::string icon_path = Glib::build_filename(libtimeit::image_path(), "icon.svg" );
	set_logo(Gdk::Pixbuf::create_from_file(icon_path, 200, 200));

	set_url_hook(
			[](AboutDialog &dialog, const Glib::ustring& link)
			{
				TimeItAboutDialog::on_link_clicked(dialog, link);
			});
	set_website("https://hoglet.github.io/TimeIT/");
	set_website_label(_("TimeIT website"));

	std::list < Glib::ustring > list_authors;
	list_authors.emplace_back("Kent Asplund");
	set_copyright("Kent Asplund");
	set_authors (list_authors);
	/* This is where you add your name so you will be credited as translator */
	set_translator_credits(_("translator-credits"));
}
}
