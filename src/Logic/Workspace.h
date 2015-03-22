/*
 * Workspace.h
 *
 *  Created on: 2008-aug-15
 *      Author: hoglet
 */

#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
#include <libwnck/libwnck.h>

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
	void findLayout();
	int numWorkspaces;
	int rows;
	int columns;
	WnckScreen* screen;
	bool isVirtual;
};

#endif /* WORKSPACE_H_ */
