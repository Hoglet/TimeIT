#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <libtimeit/db/database.h>
#include <libtimeit/logic/time_keeper.h>
#include <libtimeit/timer.h>
#include <gtkmm.h>
#include "widget_interface.h"
#include "status_icon.h"
#include <gui/images.h>

namespace gui
{
using namespace libtimeit;
using namespace std;

enum widget_type
{
	MAIN_WINDOW,
	ADD_TASK_DIALOG,
	ABOUT_DIALOG,
	EDIT_TASK_DIALOG,
	ADD_TIME,
	IDLE_DIALOG,
	PREFERENCE_DIALOG,
	MAX_WIDGETS
};

using widget_ptr = std::shared_ptr<widget_interface>;

class window_manager
{
public:
	window_manager( time_manager&, database &database, Timer& timer, notification_manager& notifier, image_cache& op_images);
	virtual widget_ptr get_widget( widget_type widget);
	virtual status_icon_widget& get_status_icon();

	void manage_lifespan(shared_ptr<Gtk::Dialog> dialog);
	void quit();
private:
	widget_ptr add_task_instance;
	widget_ptr about_dialog_instance;
	widget_ptr main_window_instance;
	widget_ptr edit_task_dialog_instace;
	widget_ptr add_time_dialog_instace;
	widget_ptr preference_dialog_instance;
	status_icon_widget *status_icon_instance{nullptr};

	void on_main_window_hide();
	void on_add_task_dialog_hide();
	void on_about_dialog_response(int);
	void on_edit_task_hide();
	void on_preference_dialog_hide();

	static void on_dialog_response(Gtk::Dialog* dialog);
	void on_dialog_hide(Gtk::Dialog* dialog);


	time_manager& time_keeper;
	database&    db;
	Timer&       timer;
	notification_manager&    notifier;
	image_cache&      images;
	list<shared_ptr<Gtk::Dialog>> active_dialogs;
};
}
#endif /* WINDOW_MANAGER_H */
