#ifndef MOCKWIDGET_H_
#define MOCKWIDGET_H_

#include "IWidget.h"
#include "EditTaskDialog.h"

namespace Test
{

class MockWidget: public GUI::IAddTaskDialog, public GUI::IEditTaskDialog, public IWidget
{
public:
	MockWidget();
	virtual ~MockWidget();
	virtual void show();
	virtual void hide();
	virtual void move(int x, int y);
	virtual bool is_visible();
	virtual void get_position(int& Window_x, int& Window_y) ;
	virtual void setParent(int);
	virtual void setTaskID(int64_t);
private:
	bool visible;
};

} /* namespace Test */
#endif /* MOCKWIDGET_H_ */
