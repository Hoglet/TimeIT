/*
 * ApplicationLock.h
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#ifndef APPLICATIONLOCK_H_
#define APPLICATIONLOCK_H_
#include <fstream>
namespace libtimeit
{
using namespace std;

class ApplicationLock
{
public:
	virtual ~ApplicationLock();
	/** lockBase is the file path, including a filename, that is supposed to
	 *  be locked (in this case the database). ApplicationLock will add a
	 *  .lock to lockBase and lock the resulting file.
	 */
	ApplicationLock(string lockBase);
	bool lockAquired();
private:
	int  fdlock;
	bool locked;
};
}
#endif /* APPLICATIONLOCK_H_ */
