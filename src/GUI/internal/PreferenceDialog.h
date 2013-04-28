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
#include <Database.h>

namespace GUI
{

class PreferenceDialog: public Gtk::Dialog, public IWidget
{
public:
	PreferenceDialog(std::shared_ptr<DB::Database>& database);
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

	Gtk::CheckButton CompactLayoutButton;
	Gtk::CheckButton StartMinimizedButton;
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
	bool	startMinimized;
	int		oldGz;
	int 	oldGt;
	bool	oldCompactLayout;
	bool 	oldStartMinimized;

	void on_data_changed();

	std::shared_ptr<ISettingsAccessor> settingsAccessor;
};

}

#endif /* PREFERENCEWINDOW_H_ */
