/*
 * OSabstraction.h
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#ifndef OSABSTRACTION_H_
#define OSABSTRACTION_H_

#include <string>

namespace OSAbstraction
{
	void mkDir(std::string path);
	bool fileExists(std::string filename);
	void unlink(std::string filename);
	int getPID();
	void showURL(std::string url);
};

#endif /* OSABSTRACTION_H_ */
