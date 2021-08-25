/*
 * AboutDialog.h
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <gtkmm.h>
#include <IWidget.h>
#include <gui/images.h>

namespace gui
{
class about_dialog : public Gtk::AboutDialog, public IWidget
{
public:
	about_dialog(image_cache& images);

	// IWidget interface
	void show() override { Gtk::AboutDialog::show(); }
	void hide() override { Gtk::AboutDialog::hide(); }
	void move(int x, int y) override { Gtk::AboutDialog::move(x,y); };
	bool is_visible() override { return Gtk::AboutDialog::is_visible(); } ;
	void get_position( int& window_x, int& window_y ) override { Gtk::AboutDialog::get_position( window_x, window_y); };
private:
	static void on_link_clicked(AboutDialog& about_dialog, const Glib::ustring& link);
};

}
#endif /* ABOUT_DIALOG_H */
