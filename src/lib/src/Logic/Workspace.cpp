/*
 * Workspace.cpp
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#include "libtimeit/logic/Workspace.h"
#include <iostream>
#include <string.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

//LCOV_EXCL_START
Workspace::Workspace()
{
	try
	{
		x11.getCardinal("_NET_DESKTOP_LAYOUT", 1);
	}
	catch (const GeneralException &e)
	{
		supportsLayout = false;
	}
	findLayout();
}

Workspace::~Workspace()
{
}

void Workspace::findLayout()
{
	try
	{
		isVirtual = false;
		desktopWidth = x11.getCardinal("_NET_DESKTOP_GEOMETRY", 0);
		desktopHeight = x11.getCardinal("_NET_DESKTOP_GEOMETRY", 1);
		viewportWidth = x11.getViewportWidth();
		viewportHeight = x11.getViewportHeight();
		numWorkspaces = x11.getCardinal("_NET_NUMBER_OF_DESKTOPS", 0);
		columns = 1;
		rows = numWorkspaces;

		if (supportsLayout && numWorkspaces > 1)
		{

			/*_NET_DESKTOP_LAYOUT, orientation, columns, rows, starting_corner CARDINAL[4]/32*/
			columns = x11.getCardinal("_NET_DESKTOP_LAYOUT", 1);
			if (columns == 0)
			{
				rows = x11.getCardinal("_NET_DESKTOP_LAYOUT", 2);
				if (rows != 0)
				{
					columns = (numWorkspaces + 1) / rows;
				}
				else
				{
					columns = 1;
				}
			}
			rows = x11.getCardinal("_NET_DESKTOP_LAYOUT", 2);
			if (rows == 0)
			{
				int columns = x11.getCardinal("_NET_DESKTOP_LAYOUT", 1);
				if (columns != 0)
				{
					rows = (numWorkspaces + 1) / columns;
				}
				else
				{
					rows = 1;
				}
			}
		}
		else
		{
			columns = desktopWidth / viewportWidth;
			rows = desktopHeight / viewportHeight;
			numWorkspaces = columns * rows;
			if (numWorkspaces > 1)
			{
				isVirtual = true;
			}
		}
	}
	catch (const GeneralException &e)
	{
		std::cerr << e.what();
	}
}

int Workspace::get_active()
{
	int active = 0;
	findLayout();
	if (numWorkspaces != x11.getCardinal("_NET_NUMBER_OF_DESKTOPS", 0))
	{
		int x = x11.getCardinal("_NET_DESKTOP_VIEWPORT", 0);
		int y = x11.getCardinal("_NET_DESKTOP_VIEWPORT", 1);

		int currentColumn = x / viewportWidth + 1;
		int currentRow = y / viewportHeight + 1;
		active = currentColumn * currentRow - 1;
	}
	else
	{
		try
		{
			active = x11.getCardinal("_NET_CURRENT_DESKTOP", 0);
		}
		catch (const GeneralException &e)
		{
			std::cerr << e.what();
		}
	}
	return active;
}
std::string Workspace::get_name(int workspaceNR)
{
	if (isVirtual)
	{
		workspaceNR = 0;
	}
	std::string retVal;
	try
	{
		vector<string> names = x11.getStrings("_NET_DESKTOP_NAMES");

		if ((int) names.size() > workspaceNR)
		{
			retVal = names.at(workspaceNR);
		}
	}
	catch (const GeneralException &e)
	{
		std::cerr << e.what();
	}
	return retVal;
}
int Workspace::get_numberOfColumns()
{
	findLayout();
	return columns;
}
int Workspace::get_numberOfRows()
{
	findLayout();
	return rows;
}
int Workspace::get_numberOfWorkspaces()
{
	findLayout();
	return numWorkspaces;
}

//LCOV_EXCL_STOP
