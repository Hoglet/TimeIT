#ifndef MOCKGUIFACTORY_H_
#define MOCKGUIFACTORY_H_

#include "IGUIFactory.h"
#include "Mock/MockStatusIcon.h"
#include "MockWidget.h"


namespace Test
{

class MockGuiFactory: public GUI::IGUIFactory
{
public:
	MockGuiFactory();
	virtual ~MockGuiFactory();

	GUI::WidgetPtr getWidget(GUI::EWidget) ;
	GUI::IStatusIcon& getStatusIcon() ;
	GUI::WidgetPtr getAddTime(int64_t taskID) ;

	void run();
	std::shared_ptr<MockWidget> widget;
	GUI::EWidget widgetIdentifier = GUI::MAX_WIDGETS;
private:
	MockStatusIcon msi;
};

} /* namespace Test */
#endif /* MOCKGUIFACTORY_H_ */
