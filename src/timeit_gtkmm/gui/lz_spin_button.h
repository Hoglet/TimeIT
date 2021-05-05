/*
 * LZSpinButton.h
 *
 *  Created on: 2008-aug-21
 *      Author: hoglet
 */

#ifndef LZSPINBUTTON_H_
#define LZSPINBUTTON_H_

#include <gtkmm.h>

namespace gui
{

class LZSpinButton : public Gtk::SpinButton
{
public:
	LZSpinButton();
	virtual ~LZSpinButton();
	virtual bool on_output();
};

}
#endif /* LZSPINBUTTON_H_ */
