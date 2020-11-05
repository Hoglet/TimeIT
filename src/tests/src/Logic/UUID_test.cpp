#include "gtest/gtest.h"
#include <UUID.h>
#include <iostream>

bool isHex(const char &c)
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
bool rangeTestG4(const char &c)
{
	if (c == '8' || c == '9' || c == 'a' || c == 'b' || c == 'A' || c == 'B')
	{
		return true;
	}
	return false;
}

TEST(UUID, randomUUID)
{
	//As UUID is random it is not repeatable
	std::string uuid = UUID::randomUUID();

	ASSERT_EQ(36, uuid.length());

	//"xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
	int position = 0;
	for (position = 0; position < 8; position++)
	{
		ASSERT_TRUE( isHex(uuid[position]))<< "First group should be eight hex characters";
	}
	ASSERT_EQ('-', uuid[8])<< "Position 9 should be '-' ";
	for (position = 9; position < 13; position++)
	{
		ASSERT_TRUE(isHex(uuid[position]))<< "Second group should be four hex  characters";
	}
	ASSERT_EQ('-', uuid[13])<< "Position 14 should be '-' ";
	for (position = 14; position < 18; position++)
	{
		ASSERT_TRUE(isHex(uuid[position]))<< "Third group should be four hex characters ";
	}
	ASSERT_EQ( '4', uuid[14])<< "Position 15 should be '4' ";
	ASSERT_EQ('-', uuid[18])<< "Position 19 should be '-' ";
	for (position = 19; position < 23; position++)
	{
		ASSERT_TRUE(isHex(uuid[position]))<< "Fourth groups should be four hex characters ";
	}
	ASSERT_TRUE( rangeTestG4(uuid[19]))<< "Fourth groups first character should be 8, 9, A, or B";
	ASSERT_EQ('-', uuid[23])<< "Position 24 should be '-' ";
	for (position = 24; position < 36; position++)
	{
		ASSERT_TRUE(isHex(uuid[position]))<< "Last group should be twelve hex characters";
	}

}

void UUID_isValid()
{
	ASSERT_TRUE(UUID::isValid("007300de-0104-4084-a1dc-009200Ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4084-a1gc-009200Ca01c4"));
	ASSERT_FALSE(UUID::isValid("007g00de-0104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0i04-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4o84-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-5084-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4084-11dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4084-a1dc-00w200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de40104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-010444084-a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-40844a1dc-009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4084-a1dc4009200ca01c4"));
	ASSERT_FALSE(UUID::isValid("007300de-0104-4084-a1dc-009200ca01c44"));
	ASSERT_FALSE(UUID::isValid("07300de-0104-4084-a1dc-009200ca01c4"));

}

