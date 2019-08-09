/*
 * AboutDialog.cpp
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#include "TimeItAboutDialog.h"
#include <OSAbstraction.h>
#include <glibmm/i18n.h>
#include <Utils.h>
#include "version.h"
namespace GUI
{

TimeItAboutDialog::~TimeItAboutDialog()
{
}
void TimeItAboutDialog::on_link_clicked(AboutDialog &about_dialog, const Glib::ustring &link)
{
	OSAbstraction::showURL(link);
}

TimeItAboutDialog::TimeItAboutDialog()
{
	//set_transient_for(*this);

	set_name("TimeIt");
	set_version(VERSION);

	set_copyright("Kent Asplund");
	//Tagline
	set_comments(_("The unobtrusive time tracker\n"));
	//This is the licence text
	set_license(_("This program is licenced under GNU General Public Licence (GPL) version 2."));

	std::string iconPath = Glib::build_filename(Utils::getImagePath(), "icon.svg");
	set_logo(Gdk::Pixbuf::create_from_file(iconPath, 200, 200));

	set_url_hook(sigc::mem_fun(this, &TimeItAboutDialog::on_link_clicked));
	set_website("http://hoglet.github.io/TimeIT/");
	set_website_label(_("TimeIT website"));

	std::list < Glib::ustring > list_authors;
	list_authors.push_back("Kent Asplund");
	set_copyright("Kent Asplund");
	set_authors (list_authors);
	/* This is where you add your name so you will be credited as translator */
	set_translator_credits(_("translator-credits"));
}
}
