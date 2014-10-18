#include "ISummary.h"

namespace GUI
{

SummaryObserver::SummaryObserver()
{
	unsubscription_allowed = true;
}
SummaryObserver::~SummaryObserver()
{
	unsubscription_allowed = false;
	std::list<ISummary*>::iterator iter = subjects.begin();
	while (iter != subjects.end())
	{
		ISummary* subject = *iter;
		subject->detach(this);
		++iter;
	}
}
void SummaryObserver::attach(ISummary* subject)
{
	subjects.push_back(subject);
}
void SummaryObserver::detach(ISummary* subject)
{
	if (unsubscription_allowed)
	{
		subjects.remove(subject);
	}
}

}
