//
// Created by hoglet on 29/11/2020.
//
#include <libtimeit/EventObserver.h>

namespace libtimeit
{

using std::string;

EventObserver::~EventObserver()
{

}

void EventObserver::on_taskParentChanged(int64_t)
{
}

void EventObserver::on_message(EventType type, const string headline, const string message) const
{

}

}