#include "summary.h"
#include "libtimeit/utils.h"
#include <glibmm/i18n.h>

using namespace std;
using namespace Gtk;
using namespace Glib;
using namespace libtimeit;

namespace gui
{
using namespace std;
using namespace libtimeit;


summary_observer::~summary_observer()
{
	unsubscription_allowed = false;
	auto iter = subjects.begin();
	while (iter != subjects.end())
	{
		summary* subject = *iter;
		subject->detach(this);
		++iter;
	}
}

void summary_observer::attach(summary* subject)
{
	subjects.push_back(subject);
}

void summary_observer::detach(summary* subject)
{
	if (unsubscription_allowed)
	{
		subjects.remove(subject);
	}
}

summary::summary(
		database &db,
		notification_manager &notifier)
		:
		event_observer(notifier),
		times(db),
		tasks(db)
{
	tree_model = TreeStore::create(columns);
	set_model(tree_model);
	append_column("Name", columns.col_name);
	append_column("Time", columns.col_time);
	set_headers_visible(false);

	ref_tree_selection = get_selection();
	ref_tree_selection->signal_changed().connect(sigc::mem_fun(*this, &summary::on_selection_changed));

	Gtk::Menu::MenuList &menu_list = menu_popup.items();

	menu_list.push_back(Gtk::Menu_Helpers::MenuElem(_("Show details"), sigc::mem_fun(*this,
							&summary::on_menu_show_details)));

}


bool summary::on_button_press_event(GdkEventButton* event)
{
	bool return_value = Gtk::TreeView::on_button_press_event(event);
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		menu_popup.popup(event->button, event->time);
		return_value = true; //It has been handled.
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		on_menu_show_details();
		return_value = true;
	}
	return return_value;
}

void summary::on_menu_show_details()
{
	auto id = selected_id();
	if(id.has_value())
	{
		for (auto* observer: observers)
		{
			observer->on_show_details_clicked(id.value(), start_time, stop_time);
		}
	}
}

void summary::init()
{
}

void summary::on_selection_changed()
{
	auto id = selected_id();
	for (auto* observer: observers)
	{
		observer->on_selection_changed(id, start_time, stop_time);
	}
	if ( id.has_value() )
	{
		global_id = id;
	}

}

void summary::attach(summary_observer* observer)
{
	observers.push_back(observer);
}

void summary::detach(summary_observer* observer)
{
	observer->detach(this);
	observers.remove(observer);
}

optional<task_id> summary::selected_id()
{
	auto tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = tree_selection->get_selected();
	if (iter) //If anything is selected
	{
		TreeModel::Row row;
		row = *iter;
		return row[columns.col_id];
	}
	return {};
}

void summary::set_references(Gtk::Calendar &cal)
{
	calendar = &cal;
	connect_signals();
	on_date_changed();
}

void summary::connect_signals()
{
	calendar->signal_day_selected().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_day_selected_double_click().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_month_changed().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_next_month().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_next_year().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_prev_month().connect(sigc::mem_fun(this, &summary::on_date_changed));
	calendar->signal_prev_year().connect(sigc::mem_fun(this, &summary::on_date_changed));
}

void summary::calculate_time_span()
{
}

void summary::on_task_updated(const task_id& taskID)
{
	if (is_visible())
	{
		auto updated_task = tasks.by_id(taskID);
		Gtk::TreeIter iter = find_row(taskID);
		if (updated_task.has_value() && iter != tree_model->children().end())
		{
			TreeModel::Row row = *iter;
			time_t total_time = times.total_cumulative_time(taskID, start_time, stop_time);
			assign_values_to_row(row, *updated_task, total_time);
			if (updated_task->parent_id.has_value())
			{
				on_task_updated(updated_task->parent_id.value());
			}
		}
		else
		{
			empty();
			populate();
		}
	}
	else
	{
		needs_re_population = true;
	}
}

void summary::on_task_name_changed(const task& item)
{
	on_task_updated(item.id);
}

void summary::on_task_time_changed(const task_id& id)
{
	on_task_updated(id);
}

void summary::on_complete_update()
{
	if (is_visible())
	{
		empty();
		populate();
	}
	else
	{
		needs_re_population = true;
	}
}

void summary::on_task_removed(const task& item)
{
	Gtk::TreeIter iter = find_row(item.id);
	if (iter != tree_model->children().end())
	{
		tree_model->erase(iter);
	}
}

void summary::on_date_changed()
{
	guint year{0};
	guint month{0};
	guint day{0};
	calendar->get_date(year, month, day);
	active_day = libtimeit::to_time(
			year, month, day); // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
	auto old_start_time = start_time;
	auto old_stop_time = stop_time;
	calculate_time_span();
	if (
			needs_re_population ||
			old_start_time != start_time ||
			old_stop_time != stop_time)
	{
		if (is_visible())
		{
			empty();
			populate();
		}
		else
		{
			needs_re_population = true;
		}
	}
}

void summary::empty()
{
	tree_model->clear();
}

bool summary::on_focus(Gtk::DirectionType direction)
{
	bool return_value = Gtk::TreeView::on_focus(direction);
	if (needs_re_population)
	{
		empty();
		populate();
	}
	try_set_selection();
	on_selection_changed();
	return return_value;
}

bool summary::is_visible()
{
	Container* parent = this->get_parent();
	auto* notebook = dynamic_cast<Gtk::Notebook*>(parent->get_parent());
	if (notebook != nullptr)
	{
		int active_tab = notebook->get_current_page();
		Widget* active = notebook->get_nth_page(active_tab);
		if (active != parent)
		{
			return false;
		}
	}
	return true;
}

TreeModel::Row summary::add(const task_id& id)
{
	auto task_to_add = tasks.by_id(id);
	TreeModel::Row row;

	if (task_to_add->parent_id.has_value() )
	{
		Gtk::TreeIter iter = find_row(task_to_add->parent_id.value());
		if (iter != tree_model->children().end())
		{
			row = *iter;
		}
		else
		{
			row = add(task_to_add->parent_id.value());
		}
		row = *(tree_model->append((row).children()));
	}
	else
	{
		TreeModel::iterator iter = tree_model->append();
		row = *iter;
	}
	auto total_time = times.total_cumulative_time(id, start_time, stop_time);
	assign_values_to_row(row, *task_to_add, total_time);
	return row;
}

void summary::populate()
{
	if (is_visible())
	{
		auto active_tasks = times.active_tasks(start_time, stop_time);

		for (auto id : active_tasks)
		{
			add(id);
		}
		needs_re_population = false;
	}
	else
	{
		needs_re_population = true;
	}
}

void summary::assign_values_to_row(TreeModel::Row &row, task &task_, time_t total_time) const
{
	row[columns.col_id] = task_.id;
	row[columns.col_name] = task_.name;
	row[columns.col_time] = libtimeit::seconds_2_hhmm(total_time);
}

Gtk::TreeModel::iterator summary::find_row(const task_id& id)
{
	TreeModel::Children children = tree_model->children();
	return sub_search(id, children);
}

Gtk::TreeModel::iterator summary::sub_search(const task_id& id, TreeModel::Children children)
{
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		TreeModel::Row row = *iter;
		if (!row.children().empty())
		{
			auto sub_iter = sub_search(id, row.children());
			if (sub_iter != row.children().end())
			{
				iter = sub_iter;
				break;
			}
		}
		if (row[columns.col_id] == id)
		{
			break;
		}
	}
	return iter;
}

void summary::try_set_selection()
{
	auto tree_selection = get_selection();
	if( global_id.has_value())
	{
		auto row = find_row(global_id.value());
		if (row != tree_model->children().end())
		{
			tree_selection->select(row);
			return;
		}
	}
	tree_selection->unselect_all();
}

}
