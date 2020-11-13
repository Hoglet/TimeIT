#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <Misc/Utils.h>
#include <Logic/UUIDTool.h>
#include <optional>


using namespace std;

void UUIDTool::init()
{
	srand(Utils::now());
}


