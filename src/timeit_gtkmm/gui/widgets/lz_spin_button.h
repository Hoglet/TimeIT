#ifndef LZSPINBUTTON_H_
#define LZSPINBUTTON_H_

#include <gtkmm.h>

namespace gui
{

class LZSpinButton : public Gtk::SpinButton
{
public:
	bool on_output() override;
};

}
#endif /* LZSPINBUTTON_H_ */
