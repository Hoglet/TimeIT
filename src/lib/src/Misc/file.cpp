#include "libtimeit/misc/file.h"

namespace libtimeit
{

file::file( const string& filename, const string& attributes ) : fp(fopen( filename.c_str(), attributes.c_str()))
{
	if( fp == nullptr)
	{
		throw("Failed to open file");
	}
}

file::~file()
{
	fclose( fp );
}

file::operator FILE*() const
{
	return fp;
}


}