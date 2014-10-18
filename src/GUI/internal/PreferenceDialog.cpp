/*
 * PreferenceWindow.cpp
 *
 *  Created on: Jul 21, 2009
 *      Author: hoglet
 */

#include "PreferenceDialog.h"
#include <glibmm/i18n.h>
#include <iostream>
#include <DefaultValues.h>

namespace GUI
{

PreferenceDialog::PreferenceDialog(std::shared_ptr<DB::IDatabase>& database) :
		CancelButton(Gtk::StockID("gtk-cancel")), OKButton(Gtk::StockID("gtk-apply")), settingsAccessor(database->getSettingsAccessor())

{
	CompactLayoutButton.set_label(_("Compact layout"));
	StartMinimizedButton.set_label(_("Start minimized"));
	PasswordEntry.set_visibility(false);

	get_vbox()->pack_start(CompactLayoutButton, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(StartMinimizedButton, Gtk::PACK_EXPAND_WIDGET, 3);

	/** Listeners **/
	CompactLayoutButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	StartMinimizedButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GzEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	QuietButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
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
	/******/
	TimeConstantTable.set_border_width(2);
	TimeConstantTable.resize(3, 2);
	{
		GzLabel.set_text(_("Zero time (m)"));
		GzEntry.set_range(1, 60);
		GzEntry.set_increments(1, 10);

		GtLabel.set_text(_("Idle time (m)"));
		GtEntry.set_range(1, 60);
		GtEntry.set_increments(1, 10);
		QuietLabel.set_text(_("Quiet mode"));

		TimeConstantTable.attach(GtLabel, 0, 1, 0, 1);
		TimeConstantTable.attach(GtEntry, 1, 2, 0, 1);
		TimeConstantTable.attach(GzLabel, 0, 1, 1, 2);
		TimeConstantTable.attach(QuietLabel, 0, 1, 2, 3);
		TimeConstantTable.attach(GtEntry, 1, 2, 0, 1);
		TimeConstantTable.attach(GzEntry, 1, 2, 1, 2);
		TimeConstantTable.attach(QuietButton, 1, 2, 2, 3);
	}
	get_vbox()->pack_start(TimeConstantTable, Gtk::PACK_EXPAND_WIDGET, 3);
	/*******/
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
	oldCompactLayout = settingsAccessor->GetBoolByName("CompactLayout", DEFAULT_COMPACTLAYOUT);
	oldStartMinimized = settingsAccessor->GetBoolByName("StartMinimized", DEFAULT_START_MINIMIZED);
	oldGt = settingsAccessor->GetIntByName("Gt", DEFAULT_GT);
	oldGz = settingsAccessor->GetIntByName("Gz", DEFAULT_GZ);
	oldQuietMode = settingsAccessor->GetBoolByName("Quiet", DEFAULT_QUIET_MODE);
	oldURL = settingsAccessor->GetStringByName("URL", DEFAULT_URL);
	oldUser = settingsAccessor->GetStringByName("Username", DEFAULT_USER);
	oldPassword = settingsAccessor->GetStringByName("Password", DEFAULT_PASSWORD);
	oldIgnoreCertErr = settingsAccessor->GetBoolByName("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	oldSyncInterval = settingsAccessor->GetIntByName("SyncInterval", DEFAULT_SYNC_INTERVAL);
	GzEntry.set_value(oldGz);
	GtEntry.set_value(oldGt);
	CompactLayoutButton.set_active(oldCompactLayout);
	StartMinimizedButton.set_active(oldStartMinimized);
	QuietButton.set_active(oldQuietMode);
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
	quietMode = QuietButton.get_active();
	URL = UrlEntry.get_text();
	User = UserEntry.get_text();
	Password = PasswordEntry.get_text();
	ignoreCertErr = IgnoreCertErrorButton.get_active();
	syncInterval = SyncIntervalEntry.get_value_as_int();
}

void PreferenceDialog::on_data_changed()
{
	get_values();
	if (gz < gt
			&& (gz != oldGz ||
			    gt != oldGt ||
			    compactLayout != oldCompactLayout ||
			    startMinimized != oldStartMinimized ||
			    oldPassword != Password ||
			    oldUser != User ||
			    oldURL != URL ||
			    oldIgnoreCertErr != ignoreCertErr ||
			    oldSyncInterval != syncInterval))
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

bool PreferenceDialog::on_button_released(GdkEventButton* event)
{
	on_data_changed();
	return false;
}

void PreferenceDialog::on_CancelButton_clicked()
{
	hide();
}
void PreferenceDialog::on_OKButton_clicked()
{
	get_values();
	if (compactLayout != oldCompactLayout)
	{
		compactLayout = settingsAccessor->SetBoolByName("CompactLayout", compactLayout);
		oldCompactLayout = compactLayout;
	}
	if (gz != oldGz)
	{
		settingsAccessor->SetIntByName("Gz", gz);
		oldGz = gz;
	}
	if (gt != oldGt)
	{
		settingsAccessor->SetIntByName("Gt", gt);
		oldGt = gt;
	}
	if (startMinimized != oldStartMinimized)
	{
		settingsAccessor->SetBoolByName("StartMinimized", startMinimized);
		oldStartMinimized = startMinimized;
	}
	if (quietMode != oldQuietMode)
	{
		settingsAccessor->SetBoolByName("Quiet", quietMode);
		quietMode = oldQuietMode;
	}
	if (oldUser != User)
	{
		settingsAccessor->SetStringByName("Username", User);
	}
	if (oldPassword != Password)
	{
		settingsAccessor->SetStringByName("Password", Password);
	}
	if (oldURL != URL)
	{
		if (URL.length() > 0)
		{
			char lastCharacter = URL[URL.length() - 1];
			if (lastCharacter != '/')
			{
				URL += '/';
			}
		}
		settingsAccessor->SetStringByName("URL", URL);
	}
	if (oldIgnoreCertErr != ignoreCertErr)
	{
		settingsAccessor->SetBoolByName("IgnoreCertErr", ignoreCertErr);
	}
	if (oldSyncInterval != syncInterval)
	{
		settingsAccessor->SetIntByName("SyncInterval", syncInterval);
	}
	hide();
}

}
