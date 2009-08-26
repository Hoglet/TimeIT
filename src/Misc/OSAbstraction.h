/*
 * OSabstraction.h
 *
 *  Created on: 2008-jul-07
 *      Author: hoglet
 */

#ifndef OSABSTRACTION_H_
#define OSABSTRACTION_H_

#include <string>

class OSAbstraction
{
public:
	static void mkDir(std::string path);
	static std::string getApplicationPath();
	static bool fileExists(std::string filename);
	static void unlink(std::string filename);
	static int getPID();
	static void showURL(std::string url);
};

#endif /* OSABSTRACTION_H_ */
