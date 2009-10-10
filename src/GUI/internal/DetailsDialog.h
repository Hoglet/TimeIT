/*
 * EditTimeEntry.h
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#ifndef DETAILSDIALOG_H_
#define DETAILSDIALOG_H_

#include <gtkmm.h>
#include "LZSpinButton.h"
#include "Details.h"
#include "Summary.h"
#include "IDetailsDialog.h"
#include <Database.h>
#include <IWidget.h>

namespace GUI
{
namespace Internal
{
class DetailsDialog: public Gtk::Dialog, public SummaryObserver, public DetailsObserver, public IDetailsDialog, public IWidget
{
public:
	static boost::shared_ptr<DetailsDialog> create(boost::shared_ptr<DB::Database>& database);
	virtual ~DetailsDialog();
	void setTimeEntryID(int64_t id);
	void set(int64_t ID,time_t startTime,time_t stopTime);

	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
private:
	DetailsDialog(boost::shared_ptr<DB::Database>& database);

	//SummaryObserver
	virtual void on_selection_changed(int64_t ID,time_t startTime,time_t stopTime);
	virtual void on_showDetailsClicked(ISummary* summary,int64_t taskId, time_t startTime, time_t stopTime) {};

	//
	void on_selected_changed();
	void setValues();
	void on_OKButton_clicked();
	void on_CancelButton_clicked();
	void on_change();
	void checkForChanges();

	Details detailList;
	Gtk::Table table;
	Gtk::Label startTimeLabel;
	Gtk::Label stopTimeLabel;
	Gtk::ScrolledWindow scrolledWindow;
	LZSpinButton startTimeHour;
	LZSpinButton startTimeMinute;
	LZSpinButton stopTimeHour;
	LZSpinButton stopTimeMinute;
	Gtk::Button CancelButton;
	Gtk::Button OKButton;
//	static DetailsDialog* instance;
	time_t  oldStartTime;
	time_t  oldStopTime;
	time_t  startTime;
	time_t  stopTime;
	time_t  rangeStart;
	time_t  rangeStop;
	int64_t id;
	int64_t timeEntryID;
	boost::weak_ptr<DetailsDialog> weak_this_ptr;
	boost::shared_ptr<ITimeAccessor> timeAccessor;
};
}
}

#endif /* DETAILSDIALOG_H_ */
