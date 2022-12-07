#ifndef CALENDAR_H
#define CALENDAR_H

#include <gtkmm.h>
#include <list>

namespace gui
{

 class calendar_widget : public Gtk::Calendar
{
public:
	calendar_widget();
	~calendar_widget() noexcept;
private:
	void mark_today();

	int marked_day = 0;
	sigc::connection connection;
 };

}
#endif /* CALENDAR_H */
