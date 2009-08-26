/*
 * PreferenceWindow.h
 *
 *  Created on: Jul 21, 2009
 *      Author: hoglet
 */

#ifndef PREFERENCEWINDOW_H_
#define PREFERENCEWINDOW_H_

#include <gtkmm/dialog.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/label.h>
#include <IWidget.h>
#include <ISettingsAccessor.h>
#include <boost/shared_ptr.hpp>
#include <Database.h>

namespace GUI
{

class PreferenceDialog: public Gtk::Dialog, public IWidget
{
public:
	PreferenceDialog(boost::shared_ptr<DB::Database>& database);
	virtual ~PreferenceDialog();
	virtual void show()
	{
		Gtk::Dialog::show();
	}
	virtual void hide()
	{
		Gtk::Dialog::hide();
	}
private:
	PreferenceDialog() {};
	void on_CancelButton_clicked();
	void on_OKButton_clicked();

	Gtk::Table TimeConstantTable;

	Gtk::CheckButton CompactLayoutButton;
	Gtk::Label	GzLabel;
	Gtk::Label	GtLabel;
	Gtk::SpinButton	GzEntry;
	Gtk::SpinButton	GtEntry;

	Gtk::Button CancelButton;
	Gtk::Button OKButton;

	void get_values();
	void set_values();
	int		gz;
	int		gt;
	bool	compactLayout;
	int		oldGz;
	int 	oldGt;
	bool	oldCompactLayout;

	void on_data_changed();

	boost::shared_ptr<ISettingsAccessor> settingsAccessor;
};

}

#endif /* PREFERENCEWINDOW_H_ */
