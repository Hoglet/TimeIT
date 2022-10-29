#ifndef LZ_SPIN_BUTTON_H
#define LZ_SPIN_BUTTON_H

#include <gtkmm.h>

namespace gui
{

class ls_spin_button : public Gtk::SpinButton
{
public:
	bool on_output() override;
};

}
#endif /* LZ_SPIN_BUTTON_H */
