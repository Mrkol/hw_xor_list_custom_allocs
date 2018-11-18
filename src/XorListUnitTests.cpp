#include "gtest/gtest.h"
#include <list>
#include "StackAllocator.hpp"
#include "XorList.hpp"


TEST(XorListUnitTests, BasicTest)
{
	XorList<int> xorlst;

	ASSERT_TRUE(xorlst.empty());

	xorlst.push_back(1);
	xorlst.push_front(2);
	ASSERT_EQ(2, *xorlst.begin());
	auto it2 = ++xorlst.begin();
	ASSERT_EQ(*it2, 1);
	xorlst.pop_back();
	ASSERT_EQ(2, *xorlst.rbegin());
	ASSERT_EQ(2, *--xorlst.rend());

	xorlst.insert_before(xorlst.end(), 4);
	xorlst.insert_after(xorlst.begin(), 3);

	ASSERT_EQ(2, *xorlst.begin());
	ASSERT_EQ(3, *++xorlst.begin());
	ASSERT_EQ(4, *xorlst.rbegin());

	ASSERT_EQ(3, xorlst.size());
	xorlst.erase(++xorlst.begin());
	ASSERT_EQ(2, *xorlst.begin());
	ASSERT_EQ(4, *++xorlst.begin());
}

TEST(XorListUnitTests, InsertionTest)
{
	const int TEST_SIZE = 100;

	XorList<int> xorlst;
	std::list<int> lst;
	for (int i = 0; i < TEST_SIZE; ++i)
	{
		xorlst.push_back(i);
		lst.push_back(i);
	}

	ASSERT_EQ(xorlst.size(), lst.size());
	
	{
		auto it1 = xorlst.begin();
		auto it2 = lst.begin();
		while (it1 != xorlst.end() && it2 != lst.end())
		{
			ASSERT_EQ(*it1, *it2);
			++it1;
			++it2;
		}
	}
	
	{
		auto it1 = xorlst.rbegin();
		auto it2 = lst.rbegin();
		while (it1 != xorlst.rend() && it2 != lst.rend())
		{
			ASSERT_EQ(*it1, *it2);
			++it1;
			++it2;
		}
	}
}

int main(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
