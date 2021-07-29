#include <gtest/gtest.h>
#include <iostream>

namespace libtimeit
{
extern bool is_valid_uuid( std::string_view uuid );
}

using namespace libtimeit;

TEST (UUID, is_valid_uuid )
{
	ASSERT_TRUE  (is_valid_uuid("007300de-0104-4084-a1dc-009200Ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4084-a1gc-009200Ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007g00de-0104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0i04-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4o84-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-5084-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4084-11dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4084-a1dc-00w200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de40104-4084-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-010444084-a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-40844a1dc-009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4084-a1dc4009200ca01c4"));
	ASSERT_FALSE (is_valid_uuid("007300de-0104-4084-a1dc-009200ca01c44"));
	ASSERT_FALSE (is_valid_uuid("07300de-0104-4084-a1dc-009200ca01c4"));
}

