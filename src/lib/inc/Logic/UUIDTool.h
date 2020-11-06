#ifndef UUID_H_
#define UUID_H_

#include <string>

class UUIDTool
{
public:
	static std::string randomUUID();
	static bool isValid(const std::string& uuid);
	static void init();
private:
	UUIDTool();
	static int random4hex();
	static int randomBetween8and11();
	static bool rangeTestG4(const char& c);
	static bool isHex(const char& c);
};

#endif /* UUID_H_ */
