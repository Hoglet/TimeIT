#ifndef X11_WORKSPACE_H
#define X11_WORKSPACE_H
#include <string>
#include "libtimeit/x_lib_accessor.h"
#include "workspace.h"

namespace libtimeit
{

using namespace std;

class x11_workspace: public workspace
{
public:
	x11_workspace();

	unsigned active() override;
	string name(unsigned workspace_nr) override;

	workspace_layout layout() override;

	bool available() override;

private:

	bool supports_layout = true;
	bool is_virtual{false};

	long  viewport_width  = 0;
	long  viewport_height = 0;
	x_lib_accessor  x_lib;

};
}
#endif /* X11_WORKSPACE_H */
