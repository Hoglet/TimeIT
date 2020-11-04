#include "MockGuiFactory.h"

using namespace GUI;

namespace Test
{

MockGuiFactory::MockGuiFactory() :
		widget(std::shared_ptr<MockWidget>(new MockWidget()))
{
}

MockGuiFactory::~MockGuiFactory()
{
}

WidgetPtr MockGuiFactory::getWidget(EWidget wi)
{
	widgetIdentifier = wi;
	return widget;
}

IStatusIcon& MockGuiFactory::getStatusIcon()
{
	return msi;
}

WidgetPtr MockGuiFactory::getAddTime(int64_t taskID)
{
	WidgetPtr retVal;
	return retVal;
}

void MockGuiFactory::run()
{

}
void MockGuiFactory::quit()
{

}

} /* namespace Test */
