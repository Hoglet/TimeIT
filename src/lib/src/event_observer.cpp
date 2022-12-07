//
// Created by hoglet on 31/03/2021.
//

#include <libtimeit/db/notifier.h>

namespace libtimeit
{

event_observer::event_observer(notification_manager &notifier) : publisher(notifier)
{
	publisher.attach(this);
}


event_observer::~event_observer()
{
	publisher.detach(this);
}

}
