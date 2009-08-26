/*
 * Workspace.cpp
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */


#include "Workspace.h"
#include <libwnck/workspace.h>
#include <iostream>
#include <gtkmm.h>
#include <string>
//#include <vector>

namespace
{
WnckWorkspace* getRightMostWorkspace()
{
	WnckScreen* wnckScreen = wnck_screen_get_default();
	WnckWorkspace* pointer = wnck_screen_get_workspace(wnckScreen, 0);
	WnckWorkspace* result=pointer;
	while(pointer)
	{
		result=pointer;
		pointer=wnck_workspace_get_neighbor(pointer, WNCK_MOTION_RIGHT);
	}
	return result;
}
WnckWorkspace* getLowestWorkspace()
{
	WnckScreen* wnckScreen = wnck_screen_get_default();
	WnckWorkspace* pointer = wnck_screen_get_workspace(wnckScreen, 0);
	WnckWorkspace* result=pointer;
	while(pointer)
	{
		result=pointer;
		pointer=wnck_workspace_get_neighbor(pointer, WNCK_MOTION_DOWN);
	}
	return result;
}
}


Workspace::Workspace()
{
	WnckScreen* wnckScreen = wnck_screen_get_default();
	WnckWorkspace* wnckWorkspace=getLowestWorkspace();

	numWorkspaces = wnck_screen_get_workspace_count(wnckScreen);
	rows = wnck_workspace_get_layout_row(wnckWorkspace) + 1;

	wnckWorkspace=getRightMostWorkspace();
	columns = wnck_workspace_get_layout_column(wnckWorkspace) + 1;
	if(rows*columns<numWorkspaces )
	{
		rows = 1;
		columns = numWorkspaces;
	}
}

Workspace::~Workspace()
{
}

int Workspace::get_active()
{
	WnckScreen* wnckScreen = wnck_screen_get_default();
	Glib::RefPtr<Gdk::Display> display = Gdk::Display::get_default();
	WnckWorkspace* wnckWorkspace = wnck_screen_get_active_workspace(wnckScreen);
	return wnck_workspace_get_number(wnckWorkspace);
}
std::string Workspace::get_name(int workspaceNR)
{
	WnckScreen* wnckScreen = wnck_screen_get_default();
	WnckWorkspace* wnckWorkspace = wnck_screen_get_workspace(wnckScreen, workspaceNR);
	std::string retVal=wnck_workspace_get_name(wnckWorkspace);
	return retVal;
}
int Workspace::get_numberOfColumns()
{
	return columns;
}
int Workspace::get_numberOfRows()
{
	return rows;
}
int Workspace::get_numberOfWorkspaces()
{
	return numWorkspaces;
}

