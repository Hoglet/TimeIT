#include "lz_spin_button.h"
#include <sstream>
#include <iomanip>

namespace gui
{
using namespace std;


bool ls_spin_button::on_output()
{
	   int value=get_value_as_int();
	   stringstream str;
	   str << setfill('0') << setw(2) << value;
	   set_text(str.str());
	   return true;
}

}
