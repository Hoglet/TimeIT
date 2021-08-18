#ifndef TIMEIT_FILE_H
#define TIMEIT_FILE_H

#include <string>

namespace libtimeit
{

using namespace std;


class file
{
public:
	file( const string& filename, const string& attributes );
	file( const file & ) = delete;
	file( file && )      = delete;
	~file();

	explicit operator FILE*() const;

	file & operator=( const file & ) = delete;
	file & operator=( file && )      = delete;
private:
	FILE* fp {nullptr};


};

}
#endif //TIMEIT_FILE_H
