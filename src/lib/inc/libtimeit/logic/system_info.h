

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <utility>
#include <chrono>
#include <memory>
#include "idle_detector.h"
#include "workspace.h"

namespace libtimeit
{

using namespace std::chrono;
using namespace std;

class system_info
{
public:
	system_info();

	seconds          time_idle( );
	unsigned         current_workspace();
	workspace_layout layout( );
	string workspace_name( unsigned int i );
private:
	unique_ptr<idle_detector> idle_status;
	unique_ptr<workspace>     workspace_info;
};

} // libtimeit

#endif //SYSTEM_INFO_H
