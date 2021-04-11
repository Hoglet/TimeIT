#include <gtest/gtest.h>
#include <libtimeit.h>


int main(int argc, char **argv)
{
	libtimeit::init();
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
