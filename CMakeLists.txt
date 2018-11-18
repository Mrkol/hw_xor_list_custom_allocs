cmake_minimum_required(VERSION 3.5)

project(allocators CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
	stack_allocator_unit_tests 
	./src/StackAllocatorUnitTests.cpp
	)

target_include_directories(stack_allocator_unit_tests PUBLIC "./include")

target_link_libraries(stack_allocator_unit_tests 
	PUBLIC GTest::GTest)

add_executable(
	xor_list_unit_tests 
	./src/XorListUnitTests.cpp
	)

target_include_directories(xor_list_unit_tests PUBLIC "./include")

target_link_libraries(xor_list_unit_tests 
	PUBLIC GTest::GTest)

add_executable(
	speed_test 
	./src/SpeedTest.cpp
	)

target_include_directories(speed_test PUBLIC "./include")
