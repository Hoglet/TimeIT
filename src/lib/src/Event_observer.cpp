//
// Created by hoglet on 31/03/2021.
//

#include <libtimeit/db/notifier.h>
#include <libtimeit/event_observer.h>

namespace libtimeit
{

Event_observer::Event_observer(Notifier &notifier) : publisher(notifier)
{
	publisher.attach(this);
}


Event_observer::~Event_observer()
{
	publisher.detach(this);
}

}