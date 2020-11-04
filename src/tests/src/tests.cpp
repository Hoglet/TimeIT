#include "gtest/gtest.h"
#include "gtkmm.h"

int main(int argc, char **argv)
{
	Gtk::Main main(argc, argv);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
