#ifndef EDIT_TIME_H
#define EDIT_TIME_H

#include <gtkmm.h>
#include <gui/widgets/lz_spin_button.h>
#include <main_window/calendar.h>
#include <libtimeit/db/database.h>
#include <libtimeit/db/task_accessor.h>
#include <libtimeit/db/time_accessor.h>
#include <IWidget.h>
#include <gui/widgets/timestamp_edit.h>


namespace gui
{
using namespace libtimeit;

/*
 * Edit and add times
 */
class edit_time_dialog: public Gtk::Dialog, public IWidget
{
public:
	edit_time_dialog( time_entry op_time_entry, database& db);
	edit_time_dialog(const edit_time_dialog&)             = delete;
	edit_time_dialog(edit_time_dialog&&)                 = delete;
	edit_time_dialog& operator=(const edit_time_dialog& ) = delete;
	edit_time_dialog& operator=(edit_time_dialog&& )     = delete;
	~edit_time_dialog() override = default;

	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position( int& window_x, int& window_y ) override { Gtk::Dialog::get_position( window_x, window_y); };
private:
	void connect_signals();
	void set_values( );
	void on_change();
	void on_response(int response_id) override;

	Gtk::Table table;
	Gtk::HBox  hbox;
	Gtk::HSeparator hseparator;

	Gtk::HBox  edit_box;
	timestamp_edit start_timestamp_edit {_("Begin")};
	timestamp_edit stop_timestamp_edit  {_("End")};

	Gtk::Label task_name_label {_("Adding time to: ")};
	Gtk::Label task_name;

	Gtk::Label to_label {"→"};

	Gtk::Label                    comment_label {_("Comment:")};
	Glib::RefPtr<Gtk::TextBuffer> comment_buffer;
	Gtk::TextView                 comment_edit ;

	Gtk::Button* ok_button;

	time_entry    item_under_edit;
	time_accessor times;
	task_accessor tasks;

	time_t original_start{0};
	time_t original_stop{0};
	string original_comment;
};
}

#endif /* EDIT_TIME_H */
