#ifndef TIMEIT_TIMESTAMP_EDIT_H
#define TIMEIT_TIMESTAMP_EDIT_H

#include <string>
#include <gtkmm.h>
#include <glibmm/i18n.h>
#include <gui/widgets/lz_spin_button.h>

namespace  gui
{
using namespace std;

class Timestamp_edit : public Gtk::Frame
{
public:
	Timestamp_edit() = delete;
	Timestamp_edit(string title);

	void set_values(time_t timestamp);
	time_t timestamp();

	sigc::signal<void> signal_changed;
private:

	Gtk::HBox  padding_box;
	Gtk::VBox  vertical_box;
	Gtk::Table date_table;

	Gtk::Label year_label {_("Year")};
	Gtk::Label month_label{_("Month")};
	Gtk::Label day_label  {_("Day")};


	LZSpinButton year;
	LZSpinButton month;
	LZSpinButton day;

	Gtk::HBox  time_box;
	Gtk::Label time_label  {_("Time:")};
	Gtk::Label colon_label {":"};

	LZSpinButton hour;
	LZSpinButton minute;

	void   set_behaviour();
	void   do_layout();
	void   connect_signals();
	void   on_change();
};
}

#endif //TIMEIT_TIMESTAMP_EDIT_H
