#include <iostream>
#include <list>
#include "StackAllocator.hpp"
#include "XorList.hpp"
#include <random>
#include <ctime>


using Integer = long long;

enum class Operation
{
	PushBack, PopBack, PushFront, PopFront
};

template<class LIST>
clock_t test(const int testSize, const int seed)
{
	std::mt19937_64 engine;
	engine.seed(seed);
	std::uniform_int_distribution<Integer> distr(0, 1ll<<32);
	std::discrete_distribution<int> opdistr({2, 2, 1, 1});

	LIST list;

	clock_t beg = clock();
	for (int i = 0; i < testSize; ++i)
	{
		Operation op = static_cast<Operation>(opdistr(engine));
		switch (op)
		{
			case Operation::PushBack:
				{
					Integer val = distr(engine);
					list.push_back(val);
				}
				break;

			case Operation::PushFront:
				{
					Integer val = distr(engine);
					list.push_front(val);
				}
				break;

			case Operation::PopBack:
				if (list.empty()) continue;
				list.pop_back();
				break;

			case Operation::PopFront:
				if (list.empty()) continue;
				list.pop_front();
				break;
		}
	}
	clock_t end = clock();

	return end - beg;
}


int main()
{
	const int SIZE = 1000000;

	int seed = clock();

	std::cout << "std::list<int, std::allocator>: ";
	std::cout << test<std::list<Integer, std::allocator<Integer>>>(SIZE, seed);
	std::cout << std::endl;

	std::cout << "std::list<int, StackAllocator>: ";
	std::cout << test<std::list<Integer, StackAllocator<Integer>>>(SIZE, seed);
	std::cout << std::endl;

	std::cout << "XorList<int, std::allocator>: ";
	std::cout << test<XorList<Integer, std::allocator<Integer>>>(SIZE, seed);
	std::cout << std::endl;

	std::cout << "XorList<int, StackAllocator>: ";
	std::cout << test<XorList<Integer, StackAllocator<Integer>>>(SIZE, seed);
	std::cout << std::endl;

	return 0;
}
