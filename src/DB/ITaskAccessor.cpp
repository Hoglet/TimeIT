
#include <ITaskAccessor.h>

namespace DB
{

TaskAccessorObserver::~TaskAccessorObserver()
{
}

void TaskAccessorObserver::on_taskParentChanged(int64_t)
{
}

ITaskAccessor::~ITaskAccessor()
{
}

}
