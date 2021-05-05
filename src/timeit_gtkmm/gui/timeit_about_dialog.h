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

namespace gui
{
class TimeItAboutDialog : public Gtk::AboutDialog, public IWidget
{
public:
	TimeItAboutDialog();

	// IWidget interface
	virtual void show() { Gtk::AboutDialog::show(); }
	virtual void hide() { Gtk::AboutDialog::hide(); }
	virtual void move(int x, int y) { Gtk::AboutDialog::move(x,y); };
	virtual bool is_visible() { return Gtk::AboutDialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::AboutDialog::get_position(Window_x, Window_y); };
private:
	void on_link_clicked(AboutDialog& about_dialog, const Glib::ustring& link);
};

}
#endif /* ABOUT_DIALOG_H_ */
