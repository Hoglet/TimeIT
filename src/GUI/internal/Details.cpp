/*
 * Details.cpp
 *
 *  Created on: 2008-jul-22
 *      Author: hoglet
 */

#include <Details.h>
#include <vector>
#include <Utils.h>
#include <DetailsDialog.h>
#include <glibmm/i18n.h>

using namespace Gtk;
using namespace std;
namespace GUI
{
namespace Internal
{
Details::Details(IDetailsDialog& detailsDialog, boost::shared_ptr<DB::Database>& database) : m_detailsDialog(detailsDialog)
{
	m_timeAccessor = database->getTimeAccessor();
	m_taskAccessor = database->getTaskAccessor();
	m_treeModel = ListStore::create(m_columns);
	set_model(m_treeModel);
	append_column("Time", m_columns.m_col_time);
	m_taskAccessor->attach(this);
	set_headers_visible(false);
	//Fill the popup menu:
	{
		Gtk::Menu::MenuList& menulist = m_Menu_Popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &Details::on_menu_file_popup_edit)));
		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &Details::on_menu_file_popup_remove)));
	}
	m_Menu_Popup.accelerate(*this);
	get_selection()->signal_changed().connect(sigc::mem_fun(*this, &Details::on_selection_changed));

}


Details::~Details()
{
	m_taskAccessor->detach(this);
}

int64_t Details::getSelectedID()
{
	int retVal = 0;
	Glib::RefPtr<TreeSelection> refTreeSelection = get_selection();
	TreeModel::iterator iter;
	iter = refTreeSelection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row = *iter;
		retVal = row[m_columns.m_col_id];
		//Do something with the row.
	}
	return retVal;
}

void Details::on_menu_file_popup_edit()
{
	int64_t selectedID = getSelectedID();
	if (selectedID > 0)
	{
		m_detailsDialog.setTimeEntryID(selectedID);
		//DetailsDialog::instance().show();
	}
}
void Details::on_menu_file_popup_remove()
{
	Gtk::MessageDialog dialog( _("Do you really want to remove this?"), false , Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
	dialog.set_secondary_text(_("Gone, gone will not come again..."));

	int result = dialog.run();

	//Handle the response:
	switch (result)
	{
	case (Gtk::RESPONSE_OK):
	{
		int64_t selectedID = getSelectedID();
		if (selectedID > 0)
		{
			m_timeAccessor->remove(selectedID);
			empty();
			populate();
			//DetailsDialog::instance().show();
		}
		break;
	}
	case (Gtk::RESPONSE_CANCEL):
		std::cout << "Cancel clicked." << std::endl;
		break;
	default:
		std::cout << "Unexpected button clicked." << std::endl;
		break;
	}

}

bool Details::on_button_press_event(GdkEventButton* event)
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = TreeView::on_button_press_event(event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		m_Menu_Popup.popup(event->button, event->time);
	}

	return return_value;
}


Gtk::TreeModel::iterator Details::findRow(int id)
{
	TreeModel::Children children = m_treeModel->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[m_columns.m_col_id] == id))
		{
			break;
		}
	}
	return iter;
}
void Details::on_taskUpdated(const Task&)
{
	populate();
}
void Details::on_taskRemoved(int64_t)
{
	empty();
	populate();
}


void Details::set(int64_t ID, time_t startTime, time_t stopTime)
{
	if (ID > 0)
	{
		m_taskID = ID;
		m_startTime = startTime;
		m_stopTime = stopTime;
		empty();
		populate();
	}
	else
	{
		m_taskID = 0;
		m_startTime = 0;
		m_stopTime = 0;
		empty();
	}
}
void Details::empty()
{
	m_treeModel->clear();
}

void Details::populate()
{
	std::vector<TimeEntry> timeList = m_timeAccessor->getDetailTimeList(m_taskID, m_startTime, m_stopTime);
	std::vector<TimeEntry>::iterator iter=timeList.begin();
	for(;iter!=timeList.end();iter++)
	{
		TimeEntry te=*iter;
		TreeModel::Row row;
		TreeModel::iterator TMIter;
		Gtk::TreeIter treeIter = findRow(te.id);
		if(treeIter==m_treeModel->children().end())
		{
			treeIter = m_treeModel->append();
		}
		row = *treeIter;
		row[m_columns.m_col_id] = te.id;
		row[m_columns.m_col_time] = Utils::createDurationString(te.start,te.stop);
	}
}

void Details::on_selection_changed()
{
	std::list<DetailsObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); iter++)
	{
		DetailsObserver* observer = *iter;
		observer->on_selected_changed();
	}
}

void Details::attach(DetailsObserver* observer)
{
	observers.push_back(observer);
}
void Details::detach(DetailsObserver* observer)
{
	observers.remove(observer);
}

}
}
