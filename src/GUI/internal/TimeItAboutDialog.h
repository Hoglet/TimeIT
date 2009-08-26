/*
 * AboutDialog.h
 *
 *  Created on: 2008-jul-11
 *      Author: hoglet
 */

#ifndef ABOUTDIALOG_H_
#define ABOUTDIALOG_H_

#include <gtkmm.h>
#include <IWidget.h>

namespace GUI
{
namespace Internal
{
class TimeItAboutDialog : public Gtk::AboutDialog, public IWidget
{
public:
	TimeItAboutDialog();
	virtual ~TimeItAboutDialog();
	virtual void show() { Gtk::AboutDialog::show(); }
	virtual void hide() { Gtk::AboutDialog::hide(); }
private:
	void on_link_clicked(AboutDialog& about_dialog, const Glib::ustring& link);
};

}
}
#endif /* ABOUTDIALOG_H_ */
