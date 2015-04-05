/*
 * Workspace.h
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#include "../Misc/X11.h"

class Workspace
{
public:
	Workspace();
	virtual ~Workspace();

	int get_active();
	std::string get_name(int workspaceNR);
	int get_numberOfColumns();
	int get_numberOfRows();
	int get_numberOfWorkspaces();
private:
	bool isVirtual;
	void findLayout();
	int numWorkspaces;
	int rows;
	int columns;
	int desktopWidth;
	int desktopHeight;
	int viewportWidth;
	int viewportHeight;
	X11 x11;
};

#endif /* WORKSPACE_H_ */
