/*
 * Workspace.h
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#include <X11Property.h>

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
	int numWorkspaces;
	X11Property x11property;
};

#endif /* WORKSPACE_H_ */
