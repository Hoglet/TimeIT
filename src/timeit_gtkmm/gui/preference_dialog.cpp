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

Preference_dialog::Preference_dialog(Database &database) :
		cancel_button(Gtk::StockID("gtk-cancel")),
		ok_button(Gtk::StockID("gtk-apply")),
		settings_accessor(database)

{
	//The compact layout option is reducing the size of the toolbar in the main window
	compact_layout_button.set_label(_("Compact layout"));
	//This option will make that no window is opened on first start of application. Only an icon in the system tray will be visible
	start_minimized_button.set_label(_("Start minimized"));
	password_entry.set_visibility(false);

	get_vbox()->pack_start(compact_layout_button, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(start_minimized_button, Gtk::PACK_EXPAND_WIDGET, 3);

	/** Listeners **/
	compact_layout_button.signal_toggled().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	start_minimized_button.signal_toggled().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	gz_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	gt_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	url_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	user_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	password_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	ignore_cert_error_button.signal_toggled().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	sync_interval_entry.signal_changed().connect(sigc::mem_fun(this, &Preference_dialog::on_data_changed));
	gz_entry.signal_focus_out_event().connect(sigc::mem_fun(this, &Preference_dialog::on_focus_changed));
	gt_entry.signal_focus_out_event().connect(sigc::mem_fun(this, &Preference_dialog::on_focus_changed));
	signal_button_release_event().connect(sigc::mem_fun(this, &Preference_dialog::on_button_released));
	signal_button_release_event().connect(sigc::mem_fun(this, &Preference_dialog::on_button_released));
	this->set_events(Gdk::BUTTON_RELEASE_MASK);

	time_constant_table.set_border_width(2);
	time_constant_table.resize(3, 2);
	{
		// "Zero time" is the minimum activity time that will be counted at work. Anything less will be ignored
		gz_label.set_text(_("Zero time (m)"));
		gz_entry.set_range(1, 60);
		gz_entry.set_increments(1, 10);

		gt_label.set_text(_("Idle time (m)"));
		gt_entry.set_range(1, 60);
		gt_entry.set_increments(1, 10);

		time_constant_table.attach(gt_label, 0, 1, 0, 1);
		time_constant_table.attach(gt_entry, 1, 2, 0, 1);
		time_constant_table.attach(gz_label, 0, 1, 1, 2);
		time_constant_table.attach(gt_entry, 1, 2, 0, 1);
		time_constant_table.attach(gz_entry, 1, 2, 1, 2);
	}
	get_vbox()->pack_start(time_constant_table, Gtk::PACK_EXPAND_WIDGET, 3);

	/***The title for the TimeIT server related settings***/
	server_label.set_text(_("Sync server:"));
	server_table.set_border_width(5);
	server_table.resize(3, 2);
	{
		url_label.set_text(_("URI: "));
		user_label.set_text(_("User: "));
		password_label.set_text(_("Password: "));
		ignore_cert_error_label.set_text(_("Ignore certificate errors"));
		sync_interval_label.set_text(_("Sync interval (minutes)"));

		url_entry.set_max_length(60);
		user_entry.set_max_length(20);
		password_entry.set_max_length(20);
		sync_interval_entry.set_range(1, 60);
		sync_interval_entry.set_increments(1, 10);

		server_table.attach(url_label, 0, 1, 0, 1);
		server_table.attach(url_entry, 1, 2, 0, 1);
		server_table.attach(user_label, 0, 1, 1, 2);
		server_table.attach(user_entry, 1, 2, 1, 2);
		server_table.attach(password_label, 0, 1, 2, 3);
		server_table.attach(password_entry, 1, 2, 2, 3);
		server_table.attach(ignore_cert_error_label, 0, 1, 3, 4);
		server_table.attach(ignore_cert_error_button, 1, 2, 3, 4);
		server_table.attach(sync_interval_label, 0, 1, 4, 5);
		server_table.attach(sync_interval_entry, 1, 2, 4, 5);

	}
	sync_server_frame.add(server_table);
	sync_server_frame.set_label_widget(server_label);
	get_vbox()->pack_start(sync_server_frame);

	/*******/
	cancel_button.set_flags(Gtk::CAN_FOCUS);
	cancel_button.signal_clicked().connect(sigc::mem_fun(this, &Preference_dialog::on_cancel_button_clicked));
	ok_button.set_sensitive(false);
	ok_button.set_flags(Gtk::CAN_FOCUS);
	ok_button.signal_clicked().connect(sigc::mem_fun(this, &Preference_dialog::on_ok_button_clicked));

	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(cancel_button);
	get_action_area()->pack_start(ok_button);

	show_all_children();

	set_values();
}


void Preference_dialog::set_values()
{
	old_compact_layout = settings_accessor.get_bool("CompactLayout", DEFAULT_COMPACT_LAYOUT);
	old_start_minimized = settings_accessor.get_bool("StartMinimized", DEFAULT_START_MINIMIZED);
	old_gt = (unsigned)settings_accessor.get_int("Gt", DEFAULT_GT);
	old_gz = (unsigned)settings_accessor.get_int("Gz", DEFAULT_GZ);
	old_url = settings_accessor.get_string("URL", DEFAULT_URL);
	old_user = settings_accessor.get_string("Username", DEFAULT_USER);
	old_password = settings_accessor.get_string("Password", DEFAULT_PASSWORD);
	old_ignore_cert_err = settings_accessor.get_bool("IgnoreCertErr", DEFAULT_IGNORE_CERT_ERR);
	old_sync_interval = (int)settings_accessor.get_int("SyncInterval", DEFAULT_SYNC_INTERVAL);
	gz_entry.set_value(old_gz);
	gt_entry.set_value(old_gt);
	compact_layout_button.set_active(old_compact_layout);
	start_minimized_button.set_active(old_start_minimized);
	url_entry.set_text(old_url);
	user_entry.set_text(old_user);
	password_entry.set_text(old_password);
	ignore_cert_error_button.set_active(old_ignore_cert_err);
	sync_interval_entry.set_value(old_sync_interval);
}

void Preference_dialog::get_values()
{
	compact_layout = compact_layout_button.get_active();
	start_minimized = start_minimized_button.get_active();
	gz = (unsigned)gz_entry.get_value_as_int();
	gt = (unsigned)gt_entry.get_value_as_int();
	url = url_entry.get_text();
	if (url.length() > 0)
	{
		char last_character = url[url.length() - 1];
		if (last_character != '/')
		{
			url += '/';
		}
	}
	user = user_entry.get_text();
	password = password_entry.get_text();
	ignore_cert_err = ignore_cert_error_button.get_active();
	sync_interval = sync_interval_entry.get_value_as_int();
}

void Preference_dialog::on_data_changed()
{
	get_values();
	if (gz < gt
		&&
		(
				gz != old_gz ||
				gt != old_gt ||
				compact_layout != old_compact_layout ||
				start_minimized != old_start_minimized ||
				old_password != password ||
				old_user != user ||
				old_url != url ||
				old_ignore_cert_err != ignore_cert_err ||
				old_sync_interval != sync_interval
		))
	{
		ok_button.set_sensitive(true);
	}
	else
	{
		ok_button.set_sensitive(false);
	}
}

bool Preference_dialog::on_focus_changed(GdkEventFocus* /*par*/)
{
	on_data_changed();
	return false;
}

bool Preference_dialog::on_button_released(GdkEventButton* /*par*/)
{
	on_data_changed();
	return false;
}

void Preference_dialog::on_cancel_button_clicked()
{
	hide();
}

void Preference_dialog::save()
{
	compact_layout = settings_accessor.set_bool("CompactLayout", compact_layout);
	old_compact_layout = compact_layout;

	settings_accessor.set_int("Gz", gz);
	old_gz = gz;

	settings_accessor.set_int("Gt", gt);
	old_gt = gt;

	settings_accessor.set_bool("StartMinimized", start_minimized);
	old_start_minimized = start_minimized;

	settings_accessor.set_string("Username", user);
	old_user = user;

	settings_accessor.set_string("Password", password);
	old_password = password;

	settings_accessor.set_string("URL", url);
	old_url = url;

	settings_accessor.set_bool("IgnoreCertErr", ignore_cert_err);
	old_ignore_cert_err = ignore_cert_err;

	settings_accessor.set_int("SyncInterval", sync_interval);
	old_sync_interval = sync_interval;
}

void Preference_dialog::on_ok_button_clicked()
{
	get_values();
	save();
	hide();
}

}
