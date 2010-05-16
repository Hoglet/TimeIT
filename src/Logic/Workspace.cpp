/*
 * Workspace.cpp
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */


#include "Workspace.h"
#include <iostream>
#include <string.h>
#include <string>


using std::vector;
using std::string;

Workspace::Workspace()
{
}

Workspace::~Workspace()
{
}

int Workspace::get_active()
{
    return x11property.get_cardinal( "_NET_CURRENT_DESKTOP", 0);
}
std::string Workspace::get_name(int workspaceNR)
{
	vector<string> names = x11property.get_strings ( "_NET_DESKTOP_NAMES");

	std::string retVal;
	if(names.size()>=workspaceNR)
	{
		retVal=names[workspaceNR];
	}
	return retVal;
}
int Workspace::get_numberOfColumns()
{
	int columns = 0;
	/*_NET_DESKTOP_LAYOUT, orientation, columns, rows, starting_corner CARDINAL[4]/32*/
    columns = x11property.get_cardinal( "_NET_DESKTOP_LAYOUT", 1);
    if(columns == 0)
    {
    	int rows = x11property.get_cardinal( "_NET_DESKTOP_LAYOUT", 2);
    	if(rows!=0)
    	{
    		columns = (x11property.get_cardinal( "_NET_NUMBER_OF_DESKTOPS", 0)+1)/rows;
    	}
    	else
    	{
    		columns=1;
    	}
    }
    return columns;

}
int Workspace::get_numberOfRows()
{
	int rows = 1;
    rows = x11property.get_cardinal( "_NET_DESKTOP_LAYOUT", 2);
    if(rows == 0)
    {
    	int columns = x11property.get_cardinal( "_NET_DESKTOP_LAYOUT", 1);
    	if(columns!=0)
    	{
    		rows= (x11property.get_cardinal( "_NET_NUMBER_OF_DESKTOPS", 0)+1)/columns;
    	}
    	else
    	{
    		rows=1;
    	}
    }
	return rows;
}
int Workspace::get_numberOfWorkspaces()
{
	return x11property.get_cardinal( "_NET_NUMBER_OF_DESKTOPS", 0);
}


