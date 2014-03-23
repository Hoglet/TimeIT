#include "cute.h"
#include <UUID.h>
#include "UUID_test.h"
#include <iostream>

namespace Test
{

bool isHex(const char& c)
{
	if (c >= '0' && c <= '9')
	{
		return true;
	}
	if (c >= 'a' && c <= 'f')
	{
		return true;
	}
	if (c >= 'A' && c <= 'F')
	{
		return true;
	}
	return false;
}
bool rangeTestG4(const char& c)
{
	if (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B')
	{
		return true;
	}
	return false;
}

void UUID_randomUUID()
{
	//As UUID is random it is not repeatable
	std::string uuid = UUID::randomUUID();

	ASSERT_EQUAL(36, uuid.length());

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	int position = 0;
	for (position = 0; position < 8; position++)
	{
		ASSERTM("First group should be eight hex characters", isHex(uuid[position]));
	}
	ASSERT_EQUALM("Position 9 should be '-' ", '-', uuid[8]);
	for (position = 9; position < 13; position++)
	{
		ASSERTM("Second group four should be four hex  characters", isHex(uuid[position]));
	}
	ASSERT_EQUALM("Position 14 should be '-' ", '-', uuid[13]);
	for (position = 14; position < 18; position++)
	{
		ASSERTM("Third group should be four hex characters ", isHex(uuid[position]));
	}
	ASSERT_EQUALM("Position 15 should be '4' ", '4', uuid[14]);
	ASSERT_EQUALM("Position 19 should be '-' ", '-', uuid[18]);
	for (position = 19; position < 23; position++)
	{
		ASSERTM("Fourth groups should be four hex characters ", isHex(uuid[position]));
	}
	ASSERTM("Fourth groups first character should be 8, 9, A, or B", rangeTestG4(uuid[19]));
	ASSERT_EQUALM("Position 24 should be '-' ", '-', uuid[23]);
	for (position = 24; position < 36; position++)
	{
		ASSERTM("Last group should be twelve hex characters", isHex(uuid[position]));
	}

}

void UUID_isValid()
{
	ASSERT_EQUALM("007300de-0104-4084-a1dc-009200Ca01c4 is valid" , true ,    UUID::isValid("007300de-0104-4084-a1dc-009200Ca01c4") );
	ASSERT_EQUALM("007300de-0104-4084-a1gc-009200Ca01c4 is invalid" , false , UUID::isValid("007300de-0104-4084-a1gc-009200Ca01c4") );
	ASSERT_EQUALM("007g00de-0104-4084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007g00de-0104-4084-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0i04-4084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-0i04-4084-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-4o84-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-4o84-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-5084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-5084-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-4084-11dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-4084-11dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-4084-a1dc-00w200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-4084-a1dc-00w200ca01c4") );
	ASSERT_EQUALM("007300de40104-4084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de40104-4084-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-010444084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-010444084-a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-40844a1dc-009200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-40844a1dc-009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-4084-a1dc4009200ca01c4 is invalid" , false , UUID::isValid("007300de-0104-4084-a1dc4009200ca01c4") );
	ASSERT_EQUALM("007300de-0104-4084-a1dc-009200ca01c44 is invalid", false , UUID::isValid("007300de-0104-4084-a1dc-009200ca01c44") );
	ASSERT_EQUALM("07300de-0104-4084-a1dc-009200ca01c4 is invalid" , false , UUID::isValid("07300de-0104-4084-a1dc-009200ca01c4") );

}

cute::suite make_suite_UUIDTest()
{
	cute::suite s;
	s.push_back(CUTE(UUID_randomUUID));
	s.push_back(CUTE(UUID_isValid));
	return s;
}

}
