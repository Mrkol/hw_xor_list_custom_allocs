#include "gtest/gtest.h"
#include <vector>
#include "StackAllocator.hpp"


TEST(StackAllocatorUnitTests, BasicTest)
{
	const size_t TEST_SIZE = 4096;
	StackAllocator<int> alloc;

	std::vector<int*> ptrs;
	ptrs.reserve(TEST_SIZE);
	for (int i = 0; i < TEST_SIZE; ++i)
	{
		ptrs.push_back(alloc.allocate(1));
		alloc.construct(ptrs.back(), i);
	}

	for (int i = 0; i < TEST_SIZE; ++i)
	{
		ASSERT_EQ(i, *ptrs[i]);
	}
}

TEST(StackAllocatorUnitTests, STLTest)
{
	std::vector<int, StackAllocator<int, 8192>> special;
	std::vector<int> normal;

	for (int i = 0; i < 2048; ++i)
	{
		special.push_back(i);
		normal.push_back(i);
	}

	for (int i = 0; i < 2048; ++i)
	{
		ASSERT_EQ(special[i], normal[i]);
	}
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
