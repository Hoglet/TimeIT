/*
 * LZSpinButton.cpp
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#include "LZSpinButton.h"
#include <sstream>
#include <iomanip>

namespace GUI
{
namespace Internal
{
using namespace std;

LZSpinButton::LZSpinButton()
{
	//Nothing needed
}

LZSpinButton::~LZSpinButton()
{
	//Nothing needed
}

bool LZSpinButton::on_output()
{
	   int value=get_value_as_int();
	   stringstream str;
	   str << setfill('0') << setw(2) << value;
	   set_text(str.str());
	   return true;
}

}
}
