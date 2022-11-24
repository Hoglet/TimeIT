#include "details.h"
#include <fmt/core.h>
#include <vector>
#include <libtimeit/utils.h>
#include <glibmm/i18n.h>
#include <optional>
#include <libtimeit/db/default_values.h>
#include <edit_time.h>

using namespace Gtk;
using namespace std;
using namespace libtimeit;

namespace gui
{

static const int SECONDS_PER_MINUTE = 60;

details::details(
		database&             db,
		notification_manager& notifier,
		window_manager&       gui_factory)
		:
		event_observer(notifier),
		times( db),
		settings( db),
		windows( gui_factory),
		db( db)
{
	tree_model = ListStore::create(columns);
	set_model(tree_model);

	append_column(_(" Day"), columns.col_morning);
	append_column(_(" Date"), columns.col_date);
	append_column(_(" Time span"), columns.col_time);
	append_column(_(" Duration"), columns.col_time_amount);
	append_column("  ∑  ", columns.col_day_total);
	append_column(_(" Idle time"), columns.col_idle);
	append_column(_(" Comment"), columns.col_comment);

	set_headers_visible(false);
	//Fill the popup menu:
	{
		//ToDo
		/*
		Gtk::Menu::MenuList &menulist = menu_popup.items();

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), sigc::mem_fun(*this, &details::on_menu_file_popup_edit)));

		Gtk::Menu_Helpers::MenuElem merge_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Merge with next"), sigc::mem_fun(*this, &details::on_menu_file_popup_merge));
		merge_menu_item = merge_menu_elem.get_child();
		menulist.push_back(merge_menu_elem);

		Gtk::Menu_Helpers::MenuElem split_menu_elem = Gtk::Menu_Helpers::MenuElem(_("_Split"), sigc::mem_fun(*this, &details::on_menu_file_popup_split));
		split_menu_item = split_menu_elem.get_child();
		menulist.push_back(split_menu_elem);

		menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Remove"), sigc::mem_fun(*this, &details::on_menu_file_popup_remove)));
		*/
	}
	menu_popup.accelerate(*this);
}


optional<time_id> details::get_selected()
{
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return row[columns.col_id];

	}
	return {};
}

list<time_id> details::get_selected_and_next()
{
	list<time_id> return_value;
	Glib::RefPtr<TreeSelection> ref_tree_selection = get_selection();
	TreeModel::iterator iter;
	iter = ref_tree_selection->get_selected();
	if (iter) // if anything is selected
	{
		TreeModel::Row row = *iter;
		return_value.push_back( row[columns.col_id]);
		//std::cout << "one row" << std::endl;
		++iter;
		if (iter) // if there is a next row
		{
			TreeModel::Row next_row = *iter;
			return_value.push_back(next_row[columns.col_id]);
			//std::cout << "two rows" << std::endl;
		}
	}
	return return_value;
}


void details::on_menu_file_popup_remove()
{
	auto selected = get_selected();
	if (selected.has_value() )
	{
		optional<time_entry> optional_time_entry = times.by_id( selected.value());
		if (optional_time_entry)
		{
			time_entry item = optional_time_entry.value();
			auto idle_gt = minutes(settings.get_int("Gt", DEFAULT_GT));
			auto idle_gz = minutes( settings.get_int("Gz", DEFAULT_GZ) );
			auto minutes_to_lose = duration_cast<minutes>( item.stop - item.start);
			std::string minutes_string = fmt::format("<span color='red'>{}</span>", minutes_to_lose.count());
			std::string secondary_text;
			if (minutes_to_lose > idle_gt || minutes_to_lose > idle_gz || minutes_to_lose < 0min)
			{
				secondary_text = fmt::format(
						_("Removing will lose {} minutes.\n\nRemoving will be permanent."),
						minutes_string.c_str());
			}
			else
			{
				secondary_text = _("Gone, gone will not come again...");
			}

			Gtk::MessageDialog dialog(_("Do you really want to remove this?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondary_text, true);

			int result = dialog.run();

			//Handle the response:
			switch (result)
			{
			case (Gtk::RESPONSE_OK):
			{
				// coded considering time_entry could be currently running
				if ( item.state == RUNNING )
				{
					// if running then keep running, starting over with zero length
					times.update( item.with_start( item.stop));
				}
				else
				{
					times.remove( item);
				}
				populate( );
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				[[fallthrough]];
			default:
				//std::cout << "Unexpected button clicked." << std::endl;
				break;
			}
		}
	}
}

bool offer_to_merge( optional<time_entry> &optional_time_entry, optional<time_entry> &optional_next_time_entry)
{
	return optional_time_entry.has_value() && optional_next_time_entry.has_value();
}

void details::on_menu_file_popup_merge()
{
	auto selected_list = get_selected_and_next();
	if (selected_list.size()>1)
	{
		optional<time_entry> optional_time_entry_0 = times.by_id( selected_list.front());
		optional<time_entry> optional_time_entry_1 = times.by_id( selected_list.back());
		if (offer_to_merge(optional_time_entry_0, optional_time_entry_1))
		{
			time_entry time_entry_0 = optional_time_entry_0.value();
			time_entry time_entry_1 = optional_time_entry_1.value();
			auto idle_gt = minutes(settings.get_int("Gt", DEFAULT_GT));
			auto idle_gz = minutes(settings.get_int("Gz", DEFAULT_GZ));

			auto minutes_to_gain = duration_cast<minutes>(time_entry_1.stop - time_entry_0.start);

			std::string minutes_string;
			if (minutes_to_gain >= 0min)
			{
				minutes_string = fmt::format("<span color='green'>{}</span>", minutes_to_gain.count() );
			}
			else
			{
				minutes_string = fmt::format("<span color='red'>{}</span>", minutes_to_gain.count() );
			}
			std::string secondary_text;
			if (
					minutes_to_gain > idle_gt ||
					minutes_to_gain > idle_gz ||
					minutes_to_gain < 0min)
			{
				secondary_text = fmt::format(
						_("Merging will add {} minutes.\n\nMerging with the next row will be permanent."),
						minutes_string.c_str());
			}
			else
			{
				secondary_text = _("Merging with the next row will be permanent.");
			}

			Gtk::MessageDialog dialog(_("Do you really want to merge?"), false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_OK_CANCEL);
			dialog.set_secondary_text(secondary_text, true);

			int dialog_result = dialog.run();

			//Handle the response:
			switch (dialog_result)
			{
			case (Gtk::RESPONSE_OK):
			{
				// coded considering time_entry_1 could be currently running
				auto new_start = time_entry_0.start;
				times.remove(time_entry_0);
				times.update(time_entry_1.with_start(new_start) );
				populate( );
				break;
			}
			case (Gtk::RESPONSE_CANCEL):
				[[fallthrough]];
			default:
				break;
			}
		}
	}
}

bool offer_to_split( time_entry &item)
{
	auto start_time = item.start;
	auto stop_time = item.stop;
	auto seconds_to_split = start_time - stop_time;
	bool across_days = is_on_different_days(start_time, stop_time);
	// at least use sufficient margins to stay clear of leap seconds, 2 * 3 = 6 is a good minimum
	return across_days || seconds_to_split > 120s; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
}

void details::on_menu_file_popup_split()
{
	auto selected = get_selected();
	if (selected.has_value())
	{
		optional<time_entry> optional_time_entry = times.by_id( selected.value());
		if (optional_time_entry)
		{
			time_entry item = optional_time_entry.value();
			auto start = item.start;
			auto stop  = item.stop;


			// coded considering time_entry could be currently running

			if (offer_to_split( item))
			{
				// ToDo Move this outside the GUI code
				time_point<system_clock> split_stop_time  = start + (stop - start) / 2 - 1s;
				auto split_start_time = split_stop_time + 2s;

				if ( is_on_different_days(start, stop) )
				{
					// use sufficient margins to stay clear of leap seconds
					auto first_day = libtimeit::localtime( start );
					first_day.tm_hour = 23;  // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					first_day.tm_min = 59;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					first_day.tm_sec = 57;   // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
					split_stop_time = system_clock::from_time_t( mktime(&first_day) );
					split_start_time = split_stop_time + 6s; // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
				}

				times.update( item.with_start( split_start_time).with_stop( stop) );
				times.create( time_entry( item.owner_id, start, split_stop_time) );
				populate( create_row_data(start, stop) );
			}
			else
			{
				// don't
			}
		}
	}
}

bool details::on_button_press_event(GdkEventButton *event)
{
	//Call base class, to allow normal handling,
	//such as allowing the row to be selected by the right-click:
	bool return_value = TreeView::on_button_press_event(event);

	//Then do our custom stuff:
	if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		set_visibility_of_context_menu();
		menu_popup.popup(event->button, event->time);
	}
	else if (event->type == GDK_2BUTTON_PRESS)
	{
		auto selected = get_selected();
		if(selected.has_value())
		{
			edit_time_entry(selected.value());
			return_value = true;
		}
	}

	return return_value;
}

void details::set_visibility_of_context_menu()
{
	auto selected_ids = get_selected_and_next();
	if( selected_ids.size()==2 )
	{
		auto selected_id = selected_ids.front();
		auto next_id = selected_ids.back();
		optional<time_entry> optional_time_entry = times.by_id( selected_id);
		optional<time_entry> optional_next_time_entry = times.by_id( next_id);
		if (optional_time_entry.has_value())
		{
			if (offer_to_merge(optional_time_entry, optional_next_time_entry))
			{
				merge_menu_item->set_sensitive( true);
			}
			else
			{
				merge_menu_item->set_sensitive( false);
			}
		}

	}
	else if ( selected_ids.size()==1 )
	{
		merge_menu_item->set_sensitive( false);
		auto selected_id = selected_ids.front();
		optional<time_entry> optional_time_entry = times.by_id( selected_id);
		if ( optional_time_entry.has_value() && offer_to_split( optional_time_entry.value() ) )
		{
			split_menu_item->set_sensitive( true);
		}
		else
		{
			split_menu_item->set_sensitive( false);
		}
	}
}

optional<Gtk::TreeModel::Row> details::find_row(const time_id& id)
{
	TreeModel::Children children = tree_model->children();
	TreeIter iter;
	for (iter = children.begin(); iter != children.end(); iter++)
	{
		if (((*iter)[columns.col_id] == id))
		{
			return *iter;
		}
	}
	return {};
}
void details::on_task_updated(const task_id& /*item*/)
{
	populate( );
}

void details::on_task_name_changed(const task& /*item*/)
{
	populate( );
}

void details::on_time_entry_changed(const time_entry& te)
{
	auto row = find_row(te.id);
	if ( row.has_value() )
	{
		auto item = times.by_id( te.id);
		if(item.has_value())
		{
			auto day =  item->start;
			auto start = beginning_of_day(day);
			auto stop = end_of_day(day);
			update( create_row_data( start, stop) );
		}
	}
	else
	{
		populate( );
	}
}

void details::on_task_removed(const task& /*id*/)
{
	populate();
}

void details::on_complete_update()
{
	populate( );
}

void details::set(optional<task_id> id, time_point<system_clock> start, time_point<system_clock> stop)
{
	if (id.has_value())
	{
		set_headers_visible(true);
		presented_task = id.value();
		start_time = start;
		stop_time = stop;
		populate( );
	}
	else
	{
		set_headers_visible(false);
		presented_task = {};
		start_time = system_clock::from_time_t(0);
		stop_time = system_clock::from_time_t(0);
		tree_model->clear();
	}
}

void details::on_selection_changed(
		optional<task_id>        id,
		time_point<system_clock> start,
		time_point<system_clock> stop)
{
	set( id, start, stop);
}



void details::update_row(TreeModel::Row& row, const row_data& data ) const
{
	row[columns.col_id]          = data.id;
	row[columns.col_morning]     = data.first_in_day ? day_of_week_abbreviation(data.start) : "";
	row[columns.col_date]        = data.first_in_day ? date_string(data.start) : "";
	row[columns.col_time]        = time_span_string(data.start, data.stop);
	row[columns.col_time_amount] = duration_string(data.start, data.stop);
	row[columns.col_comment]     = libtimeit::abbreviate_string(data.comment, 50);
	if(data.running)
	{
		row[columns.col_idle] = string("\u2003⌚");
	}
	else if( data.next_start >= data.stop)
	{
		row[columns.col_idle] = string("\u2003") + idling_string( data.stop, data.next_start );
	}
	else
	{
		row[columns.col_idle] = string("\u2003") + "⇣";
	}
	if(data.last_in_day)
	{
		auto seconds_today = data.cumulative_time;
		if (is_on_different_days(data.start, data.stop))
		{
			row[columns.col_day_total] = fmt::format("\u2003≠{} ", seconds_2_hhmm(seconds_today).c_str() );
		}
		else
		{
			row[columns.col_day_total] = fmt::format("\u2003≈{} ", seconds_2_hhmm(seconds_today).c_str());
		}
	}
}


list<row_data> details::create_row_data( time_point<system_clock> start, time_point<system_clock> stop)
{
	if( !presented_task.has_value() )
	{
		return {};
	}
	time_list times_in_range = times.by_activity( presented_task.value(), start, stop );
	auto prev_start   = system_clock::from_time_t(0);
	auto iter = times_in_range.begin();
	list<row_data> data_rows = {};
	seconds cumulative_time_for_day = 0s;
	for (; iter != times_in_range.end(); )
	{
		auto item = *iter;
		row_data data{};

		data.id           = item.id;
		data.prev_start   = prev_start;
		data.start        = item.start;
		prev_start        = data.start;
		data.stop         = item.stop;
		data.running      = item.state == RUNNING;
		data.comment      = item.comment;

		cumulative_time_for_day += duration_cast<seconds>(data.stop - data.start);
		data.cumulative_time =  cumulative_time_for_day;

		++iter;
		if( iter != times_in_range.end() )
		{
			auto next_time_entry = *iter;
			data.next_start = next_time_entry.start;
			data.first_in_day = is_on_different_days(data.prev_start, data.start);
			data.last_in_day = is_on_different_days(data.start, data.next_start);
			if(data.last_in_day)
			{
				cumulative_time_for_day = 0s;
			}
		}
		else
		{
			data.first_in_day = is_on_different_days(data.prev_start, data.start);
			data.last_in_day = true;
			data.next_start = system_clock::from_time_t(0);
		}
		data_rows.push_back(data);
	}
	return data_rows;
}

void details::populate()
{
	populate( create_row_data(start_time, stop_time) );
}
void details::populate(const list<row_data>& data_rows)
{
	tree_model->clear();

	for (auto data: data_rows )
	{
		auto tree_iter = tree_model->append();
		TreeModel::Row row = *tree_iter;

		update_row(row, data );

	}
}

void details::update(list<row_data> data_rows)
{

	for (auto data: data_rows )
	{
		//auto treeIter = m_treeModel->append();
		auto row = find_row(data.id);
		if ( row.has_value() )
		{
			update_row(*row, data);
		}
		else
		{
			on_complete_update();
		}

	}
}



void details::on_menu_file_popup_edit()
{
	auto selected = get_selected();
	if (selected.has_value())
	{
		edit_time_entry(selected.value());
	}
}



void details::edit_time_entry(time_id id)
{
	auto item = times.by_id( id);
	if( item.has_value() )
	{
		auto dialog = make_shared<gui::edit_time_dialog>( item.value(), db );
		windows.manage_lifespan(dialog);
		dialog->show();
	}
}

}
