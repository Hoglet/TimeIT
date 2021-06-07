#ifndef ADDTIME_H_
#define ADDTIME_H_

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
class Edit_time: public Gtk::Dialog, public IWidget
{
public:
	Edit_time(Time_entry time_entry, Database& database);
	Edit_time(const Edit_time&)             = delete;
	Edit_time(Edit_time&&)                 = delete;
	Edit_time& operator=(const Edit_time& ) = delete;
	Edit_time& operator=(Edit_time&& )     = delete;
	~Edit_time() override = default;

	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void connect_signals();
	void set_values( );
	void on_change();
	void on_response(int response_id) override;

	Gtk::Table table;
	Gtk::HBox  hbox;
	Gtk::HSeparator hseparator;

	Gtk::HBox  edit_box;
	Timestamp_edit start_timestamp_edit {_("Begin")};
	Timestamp_edit stop_timestamp_edit  {_("End")};

	Gtk::Label task_name_label {_("Adding time to: ")};
	Gtk::Label task_name;

	Gtk::Label to_label {"→"};

	Gtk::Button* ok_button;

	Time_entry    time_entry;
	Time_accessor time_accessor;
	Task_accessor task_accessor;

	time_t original_start{0};
	time_t original_stop{0};
};
}

#endif /* ADDTIME_H_ */
