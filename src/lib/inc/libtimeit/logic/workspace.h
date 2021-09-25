#ifndef WORKSPACE_H
#define WORKSPACE_H
#include <string>
#include "libtimeit/x_lib_accessor.h"

namespace libtimeit
{

using namespace std;

struct workspace_layout
{
	const unsigned  number_of_workspaces;
	const unsigned  rows;
	const unsigned  columns;
	workspace_layout(
			unsigned op_number_of_workspaces,
			unsigned op_rows,
			unsigned op_columns)
			:
			number_of_workspaces( op_number_of_workspaces),
			rows( op_rows),
			columns( op_columns)
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
	x_lib_accessor  x_lib;

};
}
#endif /* WORKSPACE_H */
