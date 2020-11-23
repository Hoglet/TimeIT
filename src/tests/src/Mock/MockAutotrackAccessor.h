/*
 * MockAutotrackAccessor.h
 *
 *  Created on: 28 Jul 2012
 *      Author: hoglet
 */

#ifndef MOCKAUTOTRACKACCESSOR_H_
#define MOCKAUTOTRACKACCESSOR_H_

#include <libtimeit/db/AutotrackAccessor.h>

namespace test
{
using namespace libtimeit;
using namespace std;

class MockAutotrackAccessor: public IAutotrackAccessor
{
public:
	MockAutotrackAccessor();
	virtual ~MockAutotrackAccessor();
	virtual std::vector<int64_t> getTaskIDs(int workspace);
	virtual std::vector<int> getWorkspaces(int64_t taskID);
	virtual void setWorkspaces(int64_t taskID, vector<int> workspaces);

};

} /* namespace Test */
#endif /* MOCKAUTOTRACKACCESSOR_H_ */
