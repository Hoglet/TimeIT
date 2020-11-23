#include <gtest/gtest.h>
#include <iostream>

namespace libtimeit
{
extern bool isValid(const std::string &uuid);
}

using namespace libtimeit;

TEST ( UUID, isValid )
{
	ASSERT_TRUE  (isValid("007300de-0104-4084-a1dc-009200Ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4084-a1gc-009200Ca01c4"));
	ASSERT_FALSE (isValid("007g00de-0104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0i04-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4o84-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-5084-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4084-11dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4084-a1dc-00w200ca01c4"));
	ASSERT_FALSE (isValid("007300de40104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-010444084-a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-40844a1dc-009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4084-a1dc4009200ca01c4"));
	ASSERT_FALSE (isValid("007300de-0104-4084-a1dc-009200ca01c44"));
	ASSERT_FALSE (isValid("07300de-0104-4084-a1dc-009200ca01c4"));
}

