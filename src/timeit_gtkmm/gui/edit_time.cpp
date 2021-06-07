#include "edit_time.h"
#include <ctime>
#include <utility>
#include <glibmm/i18n.h>

using namespace libtimeit;

namespace gui
{

Edit_time::Edit_time(
		Time_entry time_entry_,
		Database& database)
		:
		time_entry(std::move(time_entry_)),
		time_accessor(database),
		task_accessor(database)
{
	if(time_entry_.id!=0)
	{
		task_name_label.set_text(_("Editing time belonging to: "));
	}
	set_deletable(false);

	auto task = task_accessor.by_id(time_entry.task_id);
	if (task.has_value())
	{
		task_name.set_text(task->name);
	}
	set_values();

	//Layout
	set_resizable(false);
	get_vbox()->set_spacing(10);
	get_vbox()->pack_start(hbox, Gtk::PACK_SHRINK);
	{
		hbox.pack_start(task_name_label, Gtk::PACK_SHRINK);
		hbox.pack_start(task_name, Gtk::PACK_SHRINK);
		hbox.set_spacing(2);
	}
	get_vbox()->pack_start(hseparator);


	get_vbox()->pack_start(edit_box);
	edit_box.set_spacing(4);
	edit_box.pack_start(start_timestamp_edit);

	//to_label.set_alignment(Gtk::ALIGN_START);
	edit_box.pack_start(to_label, Gtk::PACK_SHRINK);
	edit_box.pack_start(stop_timestamp_edit);

	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	ok_button = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	ok_button->set_sensitive(false);

	connect_signals();

	show_all_children();


}

void Edit_time::connect_signals()
{
	auto slot = [this]()
	{
		this->on_change();
	};

	start_timestamp_edit.signal_changed.connect( slot );
	stop_timestamp_edit.signal_changed.connect( slot );
}


void Edit_time::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		time_t start_time = start_timestamp_edit.timestamp();
		time_t stop_time = stop_timestamp_edit.timestamp();
		if(time_entry.id>0)
		{
			time_accessor.update(time_entry.with_start(start_time).with_stop(stop_time));
		}
		else
		{
			time_accessor.create(time_entry.with_start(start_time).with_stop(stop_time));
		}
	}
}



void Edit_time::set_values()
{
	start_timestamp_edit.set_values(time_entry.start);
	stop_timestamp_edit.set_values(time_entry.stop);

	original_start = start_timestamp_edit.timestamp();
	original_stop  = stop_timestamp_edit.timestamp();
}

void Edit_time::on_change()
{
	auto start = start_timestamp_edit.timestamp();
	auto stop  = stop_timestamp_edit.timestamp();
	if ( stop > start &&
			(
				original_start != start ||
				original_stop  != stop
			)
		)
	{
		ok_button->set_sensitive(true);
	}
	else
	{
		ok_button->set_sensitive(false);
	}
}

}

