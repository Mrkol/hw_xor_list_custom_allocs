#pragma once

#include <cstdint>


template<size_t SIZE>
class Page final
{
public:
	Page(Page<SIZE>* previous = nullptr);

	~Page();

	char* dataBegin;
	char* dataCurrent;
	Page<SIZE>* previous;
};

template<size_t PAGE_SIZE>
class StackAllocatorImpl final
{
	using other = StackAllocatorImpl<PAGE_SIZE>;
public:
	StackAllocatorImpl();

	StackAllocatorImpl(const other&) = delete;
	StackAllocatorImpl(const other&&) = delete;
	other& operator=(const other&) = delete;
	other& operator=(const other&&) = delete;

	char* allocate(size_t count = 1);

	void AddOwner();
	void RemoveOwner();
	bool Alive();

	~StackAllocatorImpl();

private:
	using page_t = Page<PAGE_SIZE>;

	size_t referenceCounter_;
	Page<PAGE_SIZE>* current_;
};


template<typename T, size_t PAGE_SIZE = 4096>
class StackAllocator
{
	template<typename U, size_t>
	friend class StackAllocator;

	using impl_t = StackAllocatorImpl<PAGE_SIZE>;
public:
	template<typename U>
	using rebind = StackAllocator<U, PAGE_SIZE>;
	using other = StackAllocator<T, PAGE_SIZE>;

	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;

	StackAllocator();

	template<typename U>
	StackAllocator(const rebind<U>& that);
	
	template<typename U>
	StackAllocator(rebind<U>&& that);
	
	template<typename U>
	other& operator=(const rebind<U>& that);
	
	template<typename U>
	other& operator=(rebind<U>&& that);

	pointer allocate(size_t count = 1);
	void deallocate(pointer ptr, size_t count = 1);

	template<typename... Args>
	void construct(pointer ptr, Args&&... args);
	void destroy(pointer ptr);
	
	~StackAllocator();

private:
	impl_t* impl_;
};

#include "StackAllocator.tpp"
