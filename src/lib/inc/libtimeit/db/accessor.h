//
// Created by hoglet on 06/04/2021.
//

#ifndef TIMEIT_ACCESSOR_H
#define TIMEIT_ACCESSOR_H

namespace libtimeit
{
class Accessor
{
public:
	virtual void create_table()   = 0;
	virtual void drop_views()     = 0;
	virtual void create_views()   = 0;
	virtual void upgrade()        = 0;
};
}
#endif //TIMEIT_ACCESSOR_H
