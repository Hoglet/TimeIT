#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#include "libtimeit/X11.h"

namespace libtimeit
{

using namespace std;

struct Layout
{
	const unsigned  number_of_workspaces;
	const unsigned  rows;
	const unsigned  columns;
	Layout(
			unsigned number_of_workspaces_,
			unsigned rows_,
			unsigned columns_)
			:
			number_of_workspaces(number_of_workspaces_),
			rows(rows_),
			columns(columns_)
	{};
};
class Workspace
{
public:
	Workspace();
	virtual ~Workspace();

	unsigned active();
	string name(unsigned workspace_nr);

	Layout layout();

private:

	bool supports_layout = true;
	bool is_virtual;

	int  desktop_width;
	int  desktop_height;
	int  viewport_width;
	int  viewport_height;
	X11  x11;

};
}
#endif /* WORKSPACE_H_ */
