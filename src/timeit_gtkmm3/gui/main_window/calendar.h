#ifndef CALENDAR_H
#define CALENDAR_H

#include <gtkmm.h>
#include <list>

namespace gui
{

class calendar_observer
{
public:
	virtual void on_date_changed() = 0;
};

class calendar_widget : public Gtk::Calendar
{
public:
	calendar_widget();
	void attach(calendar_observer* observer);
	void detach(calendar_observer* observer);
private:
	void notify();
	void mark_today();

	int marked_day = 0;
	std::list<calendar_observer*> observers;
};

}
#endif /* CALENDAR_H */
