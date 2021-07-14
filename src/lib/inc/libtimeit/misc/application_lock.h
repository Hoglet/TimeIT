/*
 * ApplicationLock.h
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#ifndef APPLICATION_LOCK_H_
#define APPLICATION_LOCK_H_
#include <fstream>

namespace libtimeit
{
using namespace std;

class application_lock
{
public:
	virtual ~application_lock();
	/** lock_base is the file path, including a filename, that is supposed to
	 *  be locked (in this case the database). Application_lock will add a
	 *  .lock to lockBase and lock the resulting file.
	 */
	explicit application_lock(string lock_base);
	application_lock(const application_lock&) = delete;
	application_lock(const application_lock&&) = delete;
	application_lock& operator=(const application_lock&) = delete;
	application_lock& operator=(application_lock&&) = delete;

	[[nodiscard]] bool lock_acquired() const;
private:
	int  fd_lock;
	bool locked;
};
}
#endif /* APPLICATION_LOCK_H_ */
