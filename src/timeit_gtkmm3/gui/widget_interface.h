#ifndef WIDGET_INTERFACE_H
#define WIDGET_INTERFACE_H

#include <action_observer.h>
namespace gui
{

class widget_interface
{
public:
	virtual void show() = 0;
	virtual void hide() = 0;
	virtual void move(int x, int y) = 0;
	virtual bool is_visible() = 0;
	virtual void get_position( int &window_x, int &window_y ) = 0;

	virtual void attach(action_observer* /*observer*/) 	{};
	virtual void detach(action_observer* /*observer*/) 	{};
};

}
#endif /* WIDGET_INTERFACE_H */
