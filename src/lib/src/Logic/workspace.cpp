#include "libtimeit/logic/workspace.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace libtimeit
{
using namespace  std;
//LCOV_EXCL_START
Workspace::Workspace()
{
	try
	{
		x11.get_cardinal("_NET_DESKTOP_LAYOUT", 1);
	}
	catch (const General_exception &e)
	{
		supports_layout = false;
	}
}



Layout Workspace::layout()
{
	unsigned rows {1};
	unsigned columns {1};
	unsigned number_of_workspaces {1};
	try
	{
		is_virtual = false;
		auto desktop_width = x11.get_cardinal("_NET_DESKTOP_GEOMETRY", 0);
		auto desktop_height = x11.get_cardinal("_NET_DESKTOP_GEOMETRY", 1);
		viewport_width = x11.viewport_width();
		viewport_height = x11.viewport_height();
		number_of_workspaces = (unsigned)x11.get_cardinal("_NET_NUMBER_OF_DESKTOPS", 0);

		if (supports_layout && number_of_workspaces > 1)
		{

			/*_NET_DESKTOP_LAYOUT, orientation, columns, rows, starting_corner CARDINAL[4]/32*/
			columns = (unsigned)x11.get_cardinal("_NET_DESKTOP_LAYOUT", 1);
			if (columns == 0)
			{
				rows = (unsigned)x11.get_cardinal("_NET_DESKTOP_LAYOUT", 2);
				if (rows != 0)
				{
					columns = (number_of_workspaces + 1) / rows;
				}
				else
				{
					columns = 1;
				}
			}
			rows = (unsigned)x11.get_cardinal("_NET_DESKTOP_LAYOUT", 2);
			if (rows == 0)
			{
				if (columns != 0)
				{
					rows = (number_of_workspaces + 1) / columns;
				}
				else
				{
					rows = 1;
				}
			}
		}
		else
		{
			columns = (unsigned)(desktop_width / viewport_width);
			rows    = (unsigned)(desktop_height / viewport_height);
			number_of_workspaces = columns * rows;
			if (number_of_workspaces > 1)
			{
				is_virtual = true;
			}
		}
	}
	catch (const General_exception &e)
	{
		cerr << e.what();
	}
	return Layout(number_of_workspaces, rows, columns);
}

unsigned Workspace::active()
{
	long active = 0;
	auto layout = this->layout();
	if (layout.number_of_workspaces != x11.get_cardinal("_NET_NUMBER_OF_DESKTOPS", 0))
	{
		auto x = x11.get_cardinal("_NET_DESKTOP_VIEWPORT", 0);
		auto y = x11.get_cardinal("_NET_DESKTOP_VIEWPORT", 1);

		auto current_column = x / viewport_width + 1;
		auto current_row = y / viewport_height + 1;
		active = current_column * current_row - 1;
	}
	else
	{
		try
		{
			active = x11.get_cardinal("_NET_CURRENT_DESKTOP", 0);
		}
		catch (const General_exception &e)
		{
			cerr << e.what();
		}
	}
	return (unsigned)max(active,0L);
}

string Workspace::name(unsigned workspace_nr)
{
	if (is_virtual)
	{
		workspace_nr = 0;
	}

	try
	{
		vector<string> names = x11.get_strings("_NET_DESKTOP_NAMES");

		if ((unsigned) names.size() > workspace_nr)
		{
			return names.at(workspace_nr);
		}
	}
	catch (const General_exception &e)
	{
		cerr << e.what();
	}
	return string();
}

}
//LCOV_EXCL_STOP
