/*
 * EditTimeEntry.cpp
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#include "DetailsDialog.h"
#include "Utils.h"
#include <ctime>
#include <glibmm/i18n.h>

namespace GUI
{
namespace Internal
{
boost::shared_ptr<DetailsDialog> DetailsDialog::create(boost::shared_ptr<DB::Database>& database)
{
	boost::shared_ptr<DetailsDialog> retVal(new DetailsDialog(database));
	retVal->weak_this_ptr = boost::weak_ptr<DetailsDialog>(retVal);
	return retVal;
}

DetailsDialog::DetailsDialog(boost::shared_ptr<DB::Database>& database) :
	detailList(*this, database), table(4, 4), startTimeLabel(_("Start time")), stopTimeLabel(_("Stop time")), CancelButton(Gtk::StockID(
			"gtk-revert-to-saved")), OKButton(Gtk::StockID("gtk-apply"))
{
	timeAccessor = database->getTimeAccessor();
	startTimeHour.set_range(0, 23);
	startTimeMinute.set_range(0, 59);
	stopTimeHour.set_range(0, 23);
	stopTimeMinute.set_range(0, 59);
	startTimeHour.set_increments(1, 1);
	startTimeMinute.set_increments(1, 1);
	stopTimeHour.set_increments(1, 1);
	stopTimeMinute.set_increments(1, 1);

	scrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolledWindow.set_size_request(240, 100);

	startTime = 0;
	oldStartTime = 1;
	stopTime = 0;
	oldStopTime = 1;

	//Layout
	table.attach(startTimeLabel, 0, 2, 0, 1);
	table.attach(stopTimeLabel, 2, 4, 0, 1);
	table.attach(startTimeHour, 0, 1, 1, 2);
	table.attach(startTimeMinute, 1, 2, 1, 2);
	table.attach(stopTimeHour, 2, 3, 1, 2);
	table.attach(stopTimeMinute, 3, 4, 1, 2);

	scrolledWindow.add(detailList);
	get_vbox()->pack_start(scrolledWindow, Gtk::PACK_EXPAND_WIDGET, 3);
	get_vbox()->pack_start(table, Gtk::PACK_SHRINK, 3);
	get_action_area()->property_layout_style().set_value(Gtk::BUTTONBOX_END);
	get_action_area()->pack_start(CancelButton);
	get_action_area()->pack_start(OKButton);
	show_all_children();

	//Connect signals
	OKButton.signal_clicked().connect(sigc::mem_fun(this, &DetailsDialog::on_OKButton_clicked));
	CancelButton.signal_clicked().connect(sigc::mem_fun(this, &DetailsDialog::on_CancelButton_clicked));
	startTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	startTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	stopTimeHour.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));
	stopTimeMinute.signal_value_changed().connect(sigc::mem_fun(this, &DetailsDialog::on_change));

	detailList.attach(this);

	setTimeEntryID(0);
	checkForChanges();
}

DetailsDialog::~DetailsDialog()
{
}

void DetailsDialog::observe(ISummary* summary)
{
	summary->attach(this);
}

void DetailsDialog::on_OKButton_clicked()
{
	if (startTime != oldStartTime)
	{
		timeAccessor->changeStartTime(timeEntryID, startTime);
	}
	if (stopTimeHour.sensitive())
	{
		timeAccessor->changeEndTime(timeEntryID, stopTime);
	}
	detailList.set(id, startTime, stopTime);
	checkForChanges();
}
void DetailsDialog::on_CancelButton_clicked()
{
	setValues();
}

void DetailsDialog::on_change()
{
	struct tm startTimeInfo = *localtime(&oldStartTime);
	struct tm stopTimeInfo = *localtime(&oldStopTime);
	startTimeInfo.tm_hour = startTimeHour.get_value_as_int();
	startTimeInfo.tm_min = startTimeMinute.get_value_as_int();
	stopTimeInfo.tm_hour = stopTimeHour.get_value_as_int();
	stopTimeInfo.tm_min = stopTimeMinute.get_value_as_int();
	startTime = mktime(&startTimeInfo);
	stopTime = mktime(&stopTimeInfo);
	checkForChanges();
}

void DetailsDialog::on_selected_changed()
{
	setTimeEntryID(detailList.getSelectedID());
}

void DetailsDialog::setValues()
{
	struct tm startTimeInfo = *localtime(&oldStartTime);
	struct tm stopTimeInfo = *localtime(&oldStopTime);
	startTimeHour.set_value(startTimeInfo.tm_hour);
	startTimeMinute.set_value(startTimeInfo.tm_min);
	stopTimeHour.set_value(stopTimeInfo.tm_hour);
	stopTimeMinute.set_value(stopTimeInfo.tm_min);
}
void DetailsDialog::on_selection_changed(int64_t ID, time_t startTime, time_t stopTime)
{
	set(ID, startTime, stopTime);
}
void DetailsDialog::set(int64_t ID, time_t startTime, time_t stopTime)
{
	timeEntryID = 0;
	detailList.set(ID, startTime, stopTime);
	checkForChanges();
}

void DetailsDialog::setTimeEntryID(int64_t id)
{
	TimeEntry te = timeAccessor->getByID(id);
	if (te.id)
	{
		oldStartTime = te.start;
		startTimeHour.set_sensitive(true);
		startTimeMinute.set_sensitive(true);
		if (te.running)
		{
			stopTimeHour.set_sensitive(false);
			stopTimeMinute.set_sensitive(false);
		}
		else
		{
			stopTimeHour.set_sensitive(true);
			stopTimeMinute.set_sensitive(true);
			oldStopTime = te.stop;
		}
		timeEntryID = id;
		setValues();
	}
	else
	{
		startTimeHour.set_sensitive(false);
		startTimeMinute.set_sensitive(false);
		stopTimeHour.set_sensitive(false);
		stopTimeMinute.set_sensitive(false);
	}
}

void DetailsDialog::checkForChanges()
{
	if (timeEntryID > 0 && (oldStartTime != startTime || oldStopTime != stopTime))
	{
		OKButton.set_sensitive(true);
		CancelButton.set_sensitive(true);
	}
	else
	{
		OKButton.set_sensitive(false);
		CancelButton.set_sensitive(false);
	}
}

}
}
