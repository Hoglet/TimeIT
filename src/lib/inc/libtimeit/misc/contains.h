#ifndef TIMEIT_CONTAINS_H
#define TIMEIT_CONTAINS_H

#include <vector>

namespace libtimeit
{
using namespace std;

template <class my_type>
bool contains( vector<my_type> vect, my_type value )
{
	return any_of(
			vect.begin(),
			vect.end(),
			[value](my_type item)
			{
				return value==item;
			}
			);
}

}


#endif //TIMEIT_CONTAINS_H
