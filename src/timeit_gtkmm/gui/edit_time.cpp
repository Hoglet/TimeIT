#include "edit_time.h"
#include <ctime>
#include <utility>
#include <glibmm/i18n.h>

using namespace libtimeit;

namespace gui
{

edit_time_dialog::edit_time_dialog(
		Time_entry time_entry_,
		database& db)
		:
		time_entry(std::move(time_entry_)),
		times(db),
		tasks(db)
{
	if(time_entry_.task_uuid.has_value())
	{
		task_name_label.set_text(_("Editing time belonging to: "));
	}
	set_deletable(false);

	auto owning_task = tasks.by_id(time_entry.task_id);
	if (owning_task.has_value())
	{
		task_name.set_text(owning_task->name);
	}
	comment_buffer =Gtk:: TextBuffer::create();
	comment_edit.set_buffer(comment_buffer);
	comment_edit.set_size_request(300, 200);

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

	comment_label.set_alignment(Gtk::PACK_START);
	get_vbox()->pack_start(comment_label);
	get_vbox()->pack_start(comment_edit);

	add_button(Gtk::StockID("gtk-cancel"), Gtk::RESPONSE_CANCEL);
	ok_button = add_button(Gtk::StockID("gtk-apply"), Gtk::RESPONSE_OK);
	ok_button->set_sensitive(false);

	connect_signals();

	show_all_children();


}

void edit_time_dialog::connect_signals()
{
	auto slot = [this]()
	{
		this->on_change();
	};

	start_timestamp_edit.signal_changed.connect( slot );
	stop_timestamp_edit.signal_changed.connect( slot );
	comment_buffer->signal_changed().connect(slot);
}


void edit_time_dialog::on_response(int response_id)
{
	if (response_id == Gtk::RESPONSE_OK)
	{
		time_t start_time = start_timestamp_edit.timestamp();
		time_t stop_time  = stop_timestamp_edit.timestamp();
		auto   comment    = comment_buffer->get_text();
		auto existing_time_entry = times.by_id(time_entry.uuid);
		if(existing_time_entry.has_value())
		{
			times.update(time_entry.with_start(start_time).with_stop(stop_time).with_comment(comment));
		}
		else
		{
			times.create(time_entry.with_start(start_time).with_stop(stop_time).with_comment(comment));
		}
	}
}



void edit_time_dialog::set_values()
{
	start_timestamp_edit.set_values(time_entry.start);
	stop_timestamp_edit.set_values(time_entry.stop);

	original_start = start_timestamp_edit.timestamp();
	original_stop  = stop_timestamp_edit.timestamp();

	comment_buffer->set_text(time_entry.comment);
	original_comment = time_entry.comment;
}

void edit_time_dialog::on_change()
{
	auto start   = start_timestamp_edit.timestamp();
	auto stop    = stop_timestamp_edit.timestamp();
	auto comment = comment_buffer->get_text();
	if ( stop > start &&
			(
				original_start   != start ||
				original_stop    != stop  ||
				original_comment != comment
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

