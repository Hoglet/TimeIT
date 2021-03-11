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
#include "MainWindow/Summary.h"
#include <libtimeit/db/ISettingsAccessor.h>
#include <libtimeit/db/Database.h>
#include <IWidget.h>

namespace GUI
{
using namespace libtimeit;

class IDetailsDialog
{
public:
	virtual ~IDetailsDialog() {};
	virtual void setTimeEntryID(int64_t id) = 0;
	virtual void set(int64_t ID,time_t startTime,time_t stopTime) = 0;
	virtual void  show() = 0;
};

class DetailsDialog: public Gtk::Dialog, public SummaryObserver, public DetailsObserver, public IDetailsDialog, public IWidget
{
public:
	static std::shared_ptr<DetailsDialog> create(std::shared_ptr<IDatabase>& database);
	virtual ~DetailsDialog();
	void setTimeEntryID(int64_t id);
	void set(int64_t ID,time_t startTime,time_t stopTime);


	// IWidget interface
	virtual void show() { Gtk::Dialog::show(); }
	virtual void hide() { Gtk::Dialog::hide(); }
	virtual void move(int x, int y) { Gtk::Dialog::move(x,y); };
	virtual bool is_visible() { return Gtk::Dialog::is_visible(); } ;
	virtual void get_position(int& Window_x, int& Window_y) { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	DetailsDialog(std::shared_ptr<IDatabase>& database);

	//SummaryObserver
	virtual void on_selection_changed(int64_t ID,time_t startTime,time_t stopTime);
	virtual void on_showDetailsClicked(ISummary* summary,int64_t taskId, time_t startTime, time_t stopTime) {};

	//DetailsObserver
	void on_selected_changed();
	void on_edit_details(int64_t id);

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
	std::weak_ptr<DetailsDialog> weak_this_ptr;
	std::shared_ptr<ITimeAccessor> timeAccessor;
	std::shared_ptr<libtimeit::ISettingsAccessor> settingsAccessor;
};
}

#endif /* DETAILSDIALOG_H_ */
