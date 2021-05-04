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

	unsigned active();
	string name(unsigned workspace_nr);

	Layout layout();

private:

	bool supports_layout = true;
	bool is_virtual;

	long  viewport_width;
	long  viewport_height;
	X11  x11;

};
}
#endif /* WORKSPACE_H_ */
