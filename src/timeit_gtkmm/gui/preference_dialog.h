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
#include <gtkmm/frame.h>
#include <IWidget.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/database.h>

namespace gui
{
using namespace libtimeit;
using namespace std;

class PreferenceDialog: public Gtk::Dialog, public IWidget
{
public:
	PreferenceDialog(Database& database);

	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void on_CancelButton_clicked();
	void on_OKButton_clicked();

	Gtk::Table TimeConstantTable;
	Gtk::Table ServerTable;

	Gtk::CheckButton CompactLayoutButton;
	Gtk::CheckButton StartMinimizedButton;
	Gtk::Label	GzLabel;
	Gtk::Label	GtLabel;
	Gtk::SpinButton	GzEntry;
	Gtk::SpinButton	GtEntry;

	Gtk::Label UrlLabel;
	Gtk::Label UserLabel;
	Gtk::Label PasswordLabel;
	Gtk::Entry UrlEntry;
	Gtk::Entry UserEntry;
	Gtk::Entry PasswordEntry;
	Gtk::Label ServerLabel;
	Gtk::Frame SyncServerFrame;
	Gtk::Button CancelButton;
	Gtk::Button OKButton;
	Gtk::Label  IgnoreCertErrorLabel;
	Gtk::CheckButton IgnoreCertErrorButton;
	Gtk::Label SyncIntervalLabel;
	Gtk::SpinButton SyncIntervalEntry;

	void get_values();
	void set_values();
	int		gz;
	int		gt;
	bool	compactLayout;
	bool	startMinimized;
	bool	quietMode;
	int		oldGz;
	int 	oldGt;
	bool	oldCompactLayout;
	bool 	oldStartMinimized;
	bool	oldQuietMode;
	string  oldURL;
	string  oldUser;
	string  oldPassword;
	string  URL;
	string  User;
	string  Password;
	bool	oldIgnoreCertErr;
	bool	ignoreCertErr;
	int		oldSyncInterval;
	int		syncInterval;

	void on_data_changed();
	bool on_focus_changed(GdkEventFocus*);
	bool on_button_released(GdkEventButton* event);
	void save();

	Settings_accessor settingsAccessor;
};

}

#endif /* PREFERENCEWINDOW_H_ */
