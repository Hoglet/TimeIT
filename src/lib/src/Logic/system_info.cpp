#include "libtimeit/logic/system_info.h"
#include "x11/x11_idle_detector.h"
#include "x11/x11_workspace.h"
#include <list>

namespace libtimeit
{
using namespace std;

seconds system_info::time_idle( )
{
	return idle_status->time_idle();
}

system_info::system_info()
{
	idle_status    = make_unique<idle_detector>() ;
	workspace_info = make_unique<workspace>() ;

	list<unique_ptr<idle_detector>> idle_detectors ;
	list<unique_ptr<workspace>>     workspace_detectors ;

#if XSCREENSAVER_FOUND
	idle_detectors.emplace_back( make_unique<x11_idle_detector>() );
#endif
	//idle_detectors.emplace_back( make_unique<wayland_idle_detector>() );
	//idle_detectors.emplace_back( make_unique<mac_idle_detector>() );
	//idle_detectors.emplace_back( make_unique<win_idle_detector>() );

	workspace_detectors.emplace_back( make_unique<x11_workspace>() );
	//workspace_detectors.emplace_back( make_unique<wayland_workspace>() );
	//workspace_detectors.emplace_back( make_unique<mac_workspace>() );
	//workspace_detectors.emplace_back( make_unique<win_workspace>() );


	for (auto& detector : idle_detectors)
	{
		if (detector->available())
		{
			idle_status = move(detector);
			break;
		}
	}

	for (auto& detector : workspace_detectors)
	{
		if (detector->available())
		{
			workspace_info = move(detector);
			break;
		}
	}
}

workspace_layout system_info::layout( )
{
	return workspace_info->layout();
}

string system_info::workspace_name( unsigned int i )
{
	return workspace_info->name(i);
}

unsigned system_info::current_workspace()
{
	return workspace_info->active( );
}


} // libtimeit
