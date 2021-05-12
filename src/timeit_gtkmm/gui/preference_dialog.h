#ifndef PREFERENCE_WINDOW_H_
#define PREFERENCE_WINDOW_H_

#include <gtkmm/dialog.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/label.h>
#include <gtkmm/frame.h>
#include <IWidget.h>
#include <libtimeit/db/settings_accessor.h>
#include <libtimeit/db/database.h>

namespace gui
{
using namespace libtimeit;
using namespace std;

class Preference_dialog: public Gtk::Dialog, public IWidget
{
public:
	Preference_dialog(Database& database);

	// IWidget interface
	void show() override { Gtk::Dialog::show(); }
	void hide() override { Gtk::Dialog::hide(); }
	void move(int x, int y) override { Gtk::Dialog::move(x,y); };
	bool is_visible() override { return Gtk::Dialog::is_visible(); } ;
	void get_position(int& Window_x, int& Window_y) override { Gtk::Dialog::get_position(Window_x, Window_y); };
private:
	void on_cancel_button_clicked();
	void on_ok_button_clicked();

	Gtk::Table time_constant_table;
	Gtk::Table server_table;

	Gtk::CheckButton compact_layout_button;
	Gtk::CheckButton start_minimized_button;
	Gtk::Label	gz_label;
	Gtk::Label	gt_label;
	Gtk::SpinButton	gz_entry;
	Gtk::SpinButton	gt_entry;

	Gtk::Label url_label;
	Gtk::Label user_label;
	Gtk::Label password_label;
	Gtk::Entry url_entry;
	Gtk::Entry user_entry;
	Gtk::Entry password_entry;
	Gtk::Label server_label;
	Gtk::Frame sync_server_frame;
	Gtk::Button cancel_button;
	Gtk::Button ok_button;
	Gtk::Label  ignore_cert_error_label;
	Gtk::CheckButton ignore_cert_error_button;
	Gtk::Label sync_interval_label;
	Gtk::SpinButton sync_interval_entry;

	void get_values();
	void set_values();
	unsigned gz{0} ;
	unsigned gt{0};
	bool	 compact_layout{true};
	bool	 start_minimized{false};
	unsigned old_gz{0};
	unsigned old_gt{0};
	bool	 old_compact_layout{true};
	bool 	 old_start_minimized{false};
	string   old_url;
	string   old_user;
	string   old_password;
	string   url;
	string   user;
	string   password;
	bool	 old_ignore_cert_err{0};
	bool	 ignore_cert_err{false};
	int		 old_sync_interval{0};
	int		 sync_interval{0};

	void on_data_changed();
	bool on_focus_changed(GdkEventFocus*);
	bool on_button_released(GdkEventButton* event);
	void save();

	Settings_accessor settings_accessor;
};

}

#endif /* PREFERENCE_WINDOW_H_ */
