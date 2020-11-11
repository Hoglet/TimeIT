#include <sstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <Utils.h>
#include <UUIDTool.h>
#include <optional>


using namespace std;

void UUIDTool::init()
{
	srand(Utils::now());
}


