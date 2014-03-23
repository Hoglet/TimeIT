#include "MockWidget.h"

namespace Test
{

MockWidget::MockWidget()
{
	visible=false;
}

MockWidget::~MockWidget()
{
}

void MockWidget::show()
{
	visible=true;
}

void MockWidget::hide()
{
	visible=false;
}

void MockWidget::move(int x, int y)
{

}

bool MockWidget::is_visible()
{
	return visible;
}

void MockWidget::get_position(int& Window_x, int& Window_y)
{

}

void MockWidget::setParent(int)
{

}

void MockWidget::setTaskID(int64_t)
{

}

} /* namespace Test */
