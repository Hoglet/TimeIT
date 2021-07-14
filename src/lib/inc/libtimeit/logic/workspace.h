#ifndef WORKSPACE_H_
#define WORKSPACE_H_
#include <string>
#include "libtimeit/x11_lib.h"

namespace libtimeit
{

using namespace std;

struct workspace_layout
{
	const unsigned  number_of_workspaces;
	const unsigned  rows;
	const unsigned  columns;
	workspace_layout(
			unsigned number_of_workspaces_,
			unsigned rows_,
			unsigned columns_)
			:
			number_of_workspaces(number_of_workspaces_),
			rows(rows_),
			columns(columns_)
	{};
};
class x11_workspace
{
public:
	x11_workspace();

	unsigned active();
	string name(unsigned workspace_nr);

	workspace_layout layout();

private:

	bool supports_layout = true;
	bool is_virtual{false};

	long  viewport_width  = 0;
	long  viewport_height = 0;
	x11_lib  x11;

};
}
#endif /* WORKSPACE_H_ */
