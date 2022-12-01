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
private:
	void mark_today();

	int marked_day = 0;
};

}
#endif /* CALENDAR_H */
