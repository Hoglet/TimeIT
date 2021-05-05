/*
 * PreferenceWindow.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: hoglet
 */

#include "preference_dialog.h"
#include <glibmm/i18n.h>
#include <iostream>
#include <libtimeit/db/default_values.h>

namespace gui
{

PreferenceDialog::PreferenceDialog(Database &database) :
		CancelButton(Gtk::StockID("gtk-cancel")),
		OKButton(Gtk::StockID("gtk-apply")),
		settingsAccessor(database)

{
	//The compact layout option is reducing the size of the toolbar in the main window
	CompactLayoutButton.set_label(_("Compact layout"));
	//This option will make that no window is opened on first start of application. Only an icon in the system tray will be visible
	StartMinimizedButton.set_label(_("Start minimized"));
	PasswordEntry.set_visibility(false);

	get_vbox()->pack_start(CompactLayoutButton, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(StartMinimizedButton, Gtk::PACK_EXPAND_WIDGET, 3);

	/** Listeners **/
	CompactLayoutButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	StartMinimizedButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GzEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GtEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	UrlEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	UserEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	PasswordEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	IgnoreCertErrorButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	SyncIntervalEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GzEntry.signal_focus_out_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_focus_changed));
	GtEntry.signal_focus_out_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_focus_changed));
	signal_button_release_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_button_released));
	signal_button_release_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_button_released));
	this->set_events(Gdk::BUTTON_RELEASE_MASK);

	TimeConstantTable.set_border_width(2);
	TimeConstantTable.resize(3, 2);
	{
		// "Zero time" is the minimum activity time that will be counted at work. Anything less will be ignored
		GzLabel.set_text(_("Zero time (m)"));
		GzEntry.set_range(1, 60);
		GzEntry.set_increments(1, 10);

		GtLabel.set_text(_("Idle time (m)"));
		GtEntry.set_range(1, 60);
		GtEntry.set_increments(1, 10);

		TimeConstantTable.attach(GtLabel, 0, 1, 0, 1);
		TimeConstantTable.attach(GtEntry, 1, 2, 0, 1);
		TimeConstantTable.attach(GzLabel, 0, 1, 1, 2);
		TimeConstantTable.attach(GtEntry, 1, 2, 0, 1);
		TimeConstantTable.attach(GzEntry, 1, 2, 1, 2);
	}
	get_vbox()->pack_start(TimeConstantTable, Gtk::PACK_EXPAND_WIDGET, 3);

	/***The title for the TimeIT server related settings***/
	ServerLabel.set_text(_("Sync server:"));
	ServerTable.set_border_width(5);
	ServerTable.resize(3, 2);
	{
		UrlLabel.set_text(_("URI: "));
		UserLabel.set_text(_("User: "));
		PasswordLabel.set_text(_("Password: "));
		IgnoreCertErrorLabel.set_text(_("Ignore certificate errors"));
		SyncIntervalLabel.set_text(_("Sync interval (minutes)"));

		UrlEntry.set_max_length(60);
		UserEntry.set_max_length(20);
		PasswordEntry.set_max_length(20);
		SyncIntervalEntry.set_range(1, 60);
		SyncIntervalEntry.set_increments(1, 10);

		ServerTable.attach(UrlLabel, 0, 1, 0, 1);
		ServerTable.attach(UrlEntry, 1, 2, 0, 1);
		ServerTable.attach(UserLabel, 0, 1, 1, 2);
		ServerTable.attach(UserEntry, 1, 2, 1, 2);
		ServerTable.attach(PasswordLabel, 0, 1, 2, 3);
		ServerTable.attach(PasswordEntry, 1, 2, 2, 3);
		ServerTable.attach(IgnoreCertErrorLabel, 0, 1, 3, 4);
		ServerTable.attach(IgnoreCertErrorButton, 1, 2, 3, 4);
		ServerTable.attach(SyncIntervalLabel, 0, 1, 4, 5);
		ServerTable.attach(SyncIntervalEntry, 1, 2, 4, 5);

	}
	SyncServerFrame.add(ServerTable);
	SyncServerFrame.set_label_widget(ServerLabel);
	get_vbox()->pack_start(SyncServerFrame);

	/*******/
	CancelButton.set_flags(Gtk::CAN_FOCUS);
	CancelButton.signal_clicked().connect(sigc::mem_fun(this, &PreferenceDialog::on_CancelButton_clicked));
	OKButton.set_sensitive(false);
	OKButton.set_flags(Gtk::CAN_FOCUS);
	OKButton.signal_clicked().connect(sigc::mem_fun(this, &PreferenceDialog::on_OKButton_clicked));

	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(CancelButton);
	get_action_area()->pack_start(OKButton);

	show_all_children();

	set_values();
}

PreferenceDialog::~PreferenceDialog()
{

}

void PreferenceDialog::set_values()
{
	oldCompactLayout = settingsAccessor.get_bool("CompactLayout", DEFAULT_COMPACT_LAYOUT);
	oldStartMinimized = settingsAccessor.get_bool("StartMinimized", DEFAULT_START_MINIMIZED);
	oldGt = settingsAccessor.get_int("Gt", DEFAULT_GT);
	oldGz = settingsAccessor.get_int("Gz", DEFAULT_GZ);
	oldQuietMode = settingsAccessor.get_bool("Quiet", DEFAULT_QUIET_MODE);
	oldURL = settingsAccessor.get_string("URL", DEFAULT_URL);
	oldUser = settingsAccessor.get_string("Username", DEFAULT_USER);
	oldPassword = settingsAccessor.get_string("Password", DEFAULT_PASSWORD);
	oldIgnoreCertErr = settingsAccessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	oldSyncInterval = settingsAccessor.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	GzEntry.set_value(oldGz);
	GtEntry.set_value(oldGt);
	CompactLayoutButton.set_active(oldCompactLayout);
	StartMinimizedButton.set_active(oldStartMinimized);
	UrlEntry.set_text(oldURL);
	UserEntry.set_text(oldUser);
	PasswordEntry.set_text(oldPassword);
	IgnoreCertErrorButton.set_active(oldIgnoreCertErr);
	SyncIntervalEntry.set_value(oldSyncInterval);
}

void PreferenceDialog::get_values()
{
	compactLayout = CompactLayoutButton.get_active();
	startMinimized = StartMinimizedButton.get_active();
	gz = GzEntry.get_value_as_int();
	gt = GtEntry.get_value_as_int();
	URL = UrlEntry.get_text();
	if (URL.length() > 0)
	{
		char lastCharacter = URL[URL.length() - 1];
		if (lastCharacter != '/')
		{
			URL += '/';
		}
	}
	User = UserEntry.get_text();
	Password = PasswordEntry.get_text();
	ignoreCertErr = IgnoreCertErrorButton.get_active();
	syncInterval = SyncIntervalEntry.get_value_as_int();
}

void PreferenceDialog::on_data_changed()
{
	get_values();
	if (gz < gt
		&&
		(
			gz != oldGz ||
			gt != oldGt ||
			compactLayout != oldCompactLayout ||
			startMinimized != oldStartMinimized ||
			oldPassword != Password ||
			oldUser != User ||
			oldURL != URL ||
			oldIgnoreCertErr != ignoreCertErr ||
			oldSyncInterval != syncInterval ||
			oldQuietMode != quietMode
		))
	{
		OKButton.set_sensitive(true);
	}
	else
	{
		OKButton.set_sensitive(false);
	}
}

bool PreferenceDialog::on_focus_changed(GdkEventFocus*)
{
	on_data_changed();
	return false;
}

bool PreferenceDialog::on_button_released(GdkEventButton*)
{
	on_data_changed();
	return false;
}

void PreferenceDialog::on_CancelButton_clicked()
{
	hide();
}

void PreferenceDialog::save()
{
	compactLayout = settingsAccessor.set_bool("CompactLayout", compactLayout);
	oldCompactLayout = compactLayout;

	settingsAccessor.set_int("Gz", gz);
	oldGz = gz;

	settingsAccessor.set_int("Gt", gt);
	oldGt = gt;

	settingsAccessor.set_bool("StartMinimized", startMinimized);
	oldStartMinimized = startMinimized;

	settingsAccessor.set_bool("Quiet", quietMode);
	quietMode = oldQuietMode;

	settingsAccessor.set_string("Username", User);
	oldUser = User;

	settingsAccessor.set_string("Password", Password);
	oldPassword = Password;

	settingsAccessor.set_string("URL", URL);
	oldURL = URL;

	settingsAccessor.set_bool("IgnoreCertErr", ignoreCertErr);
	oldIgnoreCertErr = ignoreCertErr;

	settingsAccessor.set_int("SyncInterval", syncInterval);
	oldSyncInterval = syncInterval;
}

void PreferenceDialog::on_OKButton_clicked()
{
	get_values();
	save();
	hide();
}

}
