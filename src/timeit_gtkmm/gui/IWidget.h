#ifndef IWIDGET_H
#define IWIDGET_H

#include <action_observer.h>
namespace gui
{

class IWidget
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
#endif /* IWIDGET_H */
