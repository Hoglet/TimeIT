#ifndef IDLE_DETECTOR_H
#define IDLE_DETECTOR_H

#include <chrono>


namespace libtimeit
{
using namespace std::chrono;

class idle_detector
{
public:
	virtual seconds time_idle( ) { return  0s; };
	virtual bool idle( )         { return false;}
	virtual bool available()     { return false;}
};

}


#endif //IDLE_DETECTOR_H
