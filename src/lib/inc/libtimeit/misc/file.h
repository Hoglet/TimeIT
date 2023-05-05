#ifndef FILE_H
#define FILE_H

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

	operator FILE*() const;

	file & operator=( const file & ) = delete;
	file & operator=( file && )      = delete;
private:
	FILE* fp {nullptr};


};

}
#endif //FILE_H
