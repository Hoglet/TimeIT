#ifndef GTK_TIMER_H
#define GTK_TIMER_H
#include <libtimeit/timer.h>
#include <sigc++/connection.h>

namespace gui
{
using sigc::connection;
using sigc::mem_fun;

class gtk_timer : public libtimeit::timer_base
{
public:
	gtk_timer();
	gtk_timer(const gtk_timer&) = delete;
	gtk_timer( gtk_timer&& ) = delete;
	gtk_timer& operator=( const gtk_timer& ) = delete;
	gtk_timer& operator=( gtk_timer&& ) = delete;
	~gtk_timer() = default;
private:
	connection conn{};
};



}
#endif //GTK_TIMER_H
