/*
 * AboutDialog.h
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#ifndef ABOUT_DIALOG_H_
#define ABOUT_DIALOG_H_

#include <gtkmm.h>
#include <IWidget.h>
#include <gui/images.h>

namespace gui
{
class TimeItAboutDialog : public Gtk::AboutDialog, public IWidget
{
public:
	TimeItAboutDialog(Images& images);

	// IWidget interface
	void show() override { Gtk::AboutDialog::show(); }
	void hide() override { Gtk::AboutDialog::hide(); }
	void move(int x, int y) override { Gtk::AboutDialog::move(x,y); };
	bool is_visible() override { return Gtk::AboutDialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::AboutDialog::get_position(Window_x, Window_y); };
private:
	static void on_link_clicked(AboutDialog& about_dialog, const Glib::ustring& link);
};

}
#endif /* ABOUT_DIALOG_H_ */
