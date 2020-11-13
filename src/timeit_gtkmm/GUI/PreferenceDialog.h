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
#include <libtimeit/db/ISettingsAccessor.h>
#include <libtimeit/db/Database.h>

namespace GUI
{

class PreferenceDialog: public Gtk::Dialog, public IWidget
{
public:
	PreferenceDialog(std::shared_ptr<DB::IDatabase>& database);
	virtual ~PreferenceDialog();

	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	PreferenceDialog();
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

	Gtk::Label QuietLabel;
	Gtk::CheckButton QuietButton;
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
	std::string oldURL;
	std::string oldUser;
	std::string oldPassword;
	std::string URL;
	std::string User;
	std::string Password;
	bool	oldIgnoreCertErr;
	bool	ignoreCertErr;
	int		oldSyncInterval;
	int		syncInterval;

	void on_data_changed();
	bool on_focus_changed(GdkEventFocus*);
	bool on_button_released(GdkEventButton* event);
	void save();

	std::shared_ptr<DB::ISettingsAccessor> settingsAccessor;
};

}

#endif /* PREFERENCEWINDOW_H_ */
