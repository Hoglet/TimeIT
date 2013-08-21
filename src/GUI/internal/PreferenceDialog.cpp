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

PreferenceDialog::PreferenceDialog(std::shared_ptr<DB::Database>& database) :
		CancelButton(Gtk::StockID("gtk-cancel")), OKButton(Gtk::StockID("gtk-apply"))
{
	settingsAccessor = database->getSettingsAccessor();
	CompactLayoutButton.set_label(_("Compact layout"));
	StartMinimizedButton.set_label(_("Start minimized"));

	get_vbox()->pack_start(CompactLayoutButton, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(StartMinimizedButton, Gtk::PACK_EXPAND_WIDGET, 3);

	/** Listeners **/
	CompactLayoutButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	StartMinimizedButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GzEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	QuietButton.signal_toggled().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GtEntry.signal_changed().connect(sigc::mem_fun(this, &PreferenceDialog::on_data_changed));
	GzEntry.signal_focus_out_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_focus_changed));
	GtEntry.signal_focus_out_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_focus_changed));
	signal_button_release_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_button_released));
	signal_button_release_event().connect(sigc::mem_fun(this, &PreferenceDialog::on_button_released));
	this->set_events(Gdk::BUTTON_RELEASE_MASK);
	/******/
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
		TimeConstantTable.attach(GzLabel, 0, 1, 1, 2);
		TimeConstantTable.attach(QuietLabel, 0, 1, 2, 3);

		TimeConstantTable.attach(GtEntry, 1, 2, 0, 1);
		TimeConstantTable.attach(GzEntry, 1, 2, 1, 2);
		TimeConstantTable.attach(QuietButton, 1, 2, 2, 3);
	}
	get_vbox()->pack_start(TimeConstantTable);

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
	GzEntry.set_value(oldGz);
	GtEntry.set_value(oldGt);
	CompactLayoutButton.set_active(oldCompactLayout);
	StartMinimizedButton.set_active(oldStartMinimized);
	QuietButton.set_active(oldQuietMode);
}

void PreferenceDialog::get_values()
{
	compactLayout = CompactLayoutButton.get_active();
	startMinimized = StartMinimizedButton.get_active();
	gz = GzEntry.get_value_as_int();
	gt = GtEntry.get_value_as_int();
	quietMode = QuietButton.get_active();
}

void PreferenceDialog::on_data_changed()
{
	get_values();
	if (gz < gt
			&& (quietMode != oldQuietMode || gz != oldGz || gt != oldGt || compactLayout != oldCompactLayout || startMinimized != oldStartMinimized))
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
	if(quietMode != oldQuietMode)
	{
		settingsAccessor->SetBoolByName("Quiet", quietMode);
		quietMode = oldQuietMode;
	}
	hide();
}

}
