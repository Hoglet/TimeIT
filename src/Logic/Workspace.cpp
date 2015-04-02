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

using std::string;

//LCOV_EXCL_START
Workspace::Workspace()
{
	screen = wnck_screen_get_default();
	findLayout();
}

Workspace::~Workspace()
{
}

void Workspace::findLayout()
{
	GList* workspaces = wnck_screen_get_workspaces(screen);
	rows = 1;
	columns = 1;
	while (workspaces != nullptr)
	{
		WnckWorkspace* workspace = (WnckWorkspace*) workspaces->data;
		if (wnck_workspace_is_virtual(workspace))
		{
			int WSHeight = wnck_workspace_get_height(workspace);
			int WSWidth = wnck_workspace_get_width(workspace);
			int SCHeight = wnck_screen_get_height(screen);
			int SCWidth = wnck_screen_get_width(screen);
			columns = WSWidth / SCWidth;
			rows = WSHeight / SCHeight;
			numWorkspaces = columns * rows;
			isVirtual = true;
		}
		else
		{
			int r = wnck_workspace_get_layout_row(workspace) + 1;
			if (r > rows)
			{
				rows = r;
			}
			int c = wnck_workspace_get_layout_column(workspace) + 1;
			if (c > columns)
			{
				columns = c;
			}
			isVirtual = false;
		}
		workspaces = workspaces->next;
	}
	if (isVirtual == false)
	{
		numWorkspaces = wnck_screen_get_workspace_count(screen);
	}
}

int Workspace::get_active()
{
	int returnValue = 0;
	WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);
	if(wnck_workspace_is_virtual(workspace))
	{
		isVirtual = true;
		int x=wnck_workspace_get_viewport_x(workspace);
		int y=wnck_workspace_get_viewport_y(workspace);

		int scrnWidth = wnck_screen_get_width(screen);
		int scrnHeight = wnck_screen_get_height(screen);
		int currentColumn = x / scrnWidth + 1;
		int currentRow =  y / scrnHeight + 1;
		returnValue = currentColumn*currentRow -1;
	}
	else
	{
		returnValue = wnck_workspace_get_number(workspace);
	}
	return returnValue;
}
std::string Workspace::get_name(int workspaceNR)
{
	if (isVirtual)
	{
		workspaceNR = 0;
	}
	WnckWorkspace* wnckWorkspace = wnck_screen_get_workspace(screen, workspaceNR);
	return wnck_workspace_get_name(wnckWorkspace);
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
