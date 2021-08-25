#ifndef OS_ABSTRACTION_H
#define OS_ABSTRACTION_H

#include <string>

namespace libtimeit
{
using namespace std;

void make_directory(string path);
bool file_exists(string filename);
void unlink(string filename);
void show_url( string url);
}

#endif /* OS_ABSTRACTION_H */
