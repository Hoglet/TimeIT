#ifndef WORKSPACE_H
#define WORKSPACE_H
#include <string>

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
class workspace
{
public:
	virtual bool available() { return false; }

	virtual unsigned active() {return 0;};
	virtual string name(unsigned /*workspace_nr*/) { return ""; };

	virtual workspace_layout layout() { return {1,1,1};};
};
}
#endif /* WORKSPACE_H */
