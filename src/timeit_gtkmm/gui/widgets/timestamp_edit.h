#ifndef TIMESTAMP_EDIT_H
#define TIMESTAMP_EDIT_H

#include <string>
#include <gtkmm.h>
#include <glibmm/i18n.h>
#include <gui/widgets/lz_spin_button.h>

namespace  gui
{
using namespace std;
using namespace std::chrono;

class timestamp_edit : public Gtk::Frame
{
public:
	timestamp_edit() = delete;
	timestamp_edit(string title);

	void set_values( time_point<system_clock> time_stamp);
	time_point<system_clock> timestamp();

	sigc::signal<void> signal_changed;
private:

	Gtk::HBox  padding_box;
	Gtk::VBox  vertical_box;
	Gtk::Table date_table;

	Gtk::Label year_label {_("Year")};
	Gtk::Label month_label{_("Month")};
	Gtk::Label day_label  {_("Day")};


	ls_spin_button year;
	ls_spin_button month;
	ls_spin_button day;

	Gtk::HBox  time_box;
	Gtk::Label time_label  {_("Time:")};
	Gtk::Label colon_label {":"};

	ls_spin_button hour;
	ls_spin_button minute;

	void   set_behaviour();
	void   do_layout();
	void   connect_signals();
	void   on_change();
};
}

#endif //TIMESTAMP_EDIT_H
