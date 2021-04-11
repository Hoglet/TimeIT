/*
 * Workspace.cpp
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#include "libtimeit/logic/workspace.h"
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

namespace libtimeit
{

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
	find_layout();
}

Workspace::~Workspace()
{
}

void Workspace::find_layout()
{
	try
	{
		is_virtual = false;
		desktop_width = x11.get_cardinal("_NET_DESKTOP_GEOMETRY", 0);
		desktop_height = x11.get_cardinal("_NET_DESKTOP_GEOMETRY", 1);
		viewport_width = x11.viewport_width();
		viewport_height = x11.viewport_height();
		number_of_workspaces = x11.get_cardinal("_NET_NUMBER_OF_DESKTOPS", 0);
		columns = 1;
		rows = number_of_workspaces;

		if (supports_layout && number_of_workspaces > 1)
		{

			/*_NET_DESKTOP_LAYOUT, orientation, columns, rows, starting_corner CARDINAL[4]/32*/
			columns = x11.get_cardinal("_NET_DESKTOP_LAYOUT", 1);
			if (columns == 0)
			{
				rows = x11.get_cardinal("_NET_DESKTOP_LAYOUT", 2);
				if (rows != 0)
				{
					columns = (number_of_workspaces + 1) / rows;
				}
				else
				{
					columns = 1;
				}
			}
			rows = x11.get_cardinal("_NET_DESKTOP_LAYOUT", 2);
			if (rows == 0)
			{
				int columns = x11.get_cardinal("_NET_DESKTOP_LAYOUT", 1);
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
			columns = desktop_width / viewport_width;
			rows = desktop_height / viewport_height;
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
}

int Workspace::get_active()
{
	int active = 0;
	find_layout();
	if (number_of_workspaces != x11.get_cardinal("_NET_NUMBER_OF_DESKTOPS", 0))
	{
		int x = x11.get_cardinal("_NET_DESKTOP_VIEWPORT", 0);
		int y = x11.get_cardinal("_NET_DESKTOP_VIEWPORT", 1);

		int currentColumn = x / viewport_width + 1;
		int currentRow = y / viewport_height + 1;
		active = currentColumn * currentRow - 1;
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
	return active;
}

string Workspace::get_name(int workspaceNR)
{
	if (is_virtual)
	{
		workspaceNR = 0;
	}
	string retVal;
	try
	{
		vector<string> names = x11.get_strings("_NET_DESKTOP_NAMES");

		if ((int) names.size() > workspaceNR)
		{
			retVal = names.at(workspaceNR);
		}
	}
	catch (const General_exception &e)
	{
		cerr << e.what();
	}
	return retVal;
}

int Workspace::get_numberOfColumns()
{
	find_layout();
	return columns;
}

int Workspace::get_numberOfRows()
{
	find_layout();
	return rows;
}

int Workspace::get_numberOfWorkspaces()
{
	find_layout();
	return number_of_workspaces;
}

}
//LCOV_EXCL_STOP
