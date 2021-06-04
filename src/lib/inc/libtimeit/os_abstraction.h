#ifndef OS_ABSTRACTION_H_
#define OS_ABSTRACTION_H_

#include <string>

namespace libtimeit
{
using namespace std;

void make_directory(string path);
bool file_exists(string filename);
void unlink(string filename);
void show_URL(string url);
}

#endif /* OS_ABSTRACTION_H_ */
