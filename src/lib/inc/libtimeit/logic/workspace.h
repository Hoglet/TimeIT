/*
 * Workspace.h
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#include "libtimeit/X11.h"

namespace libtimeit
{

using namespace std;

class Workspace
{
public:
	Workspace();
	virtual ~Workspace();

	int get_active();
	string get_name(int workspaceNR);
	int get_numberOfColumns();
	int get_numberOfRows();
	int get_numberOfWorkspaces();

private:
	void find_layout();

	bool supports_layout = true;
	bool is_virtual;
	int  number_of_workspaces;
	int  rows;
	int  columns;
	int  desktop_width;
	int  desktop_height;
	int  viewport_width;
	int  viewport_height;
	X11  x11;

};
}
#endif /* WORKSPACE_H_ */
