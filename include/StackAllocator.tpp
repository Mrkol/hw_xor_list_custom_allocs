#include "StackAllocator.hpp"
#include <cstdlib>
#include <new>


template<size_t SIZE>
Page<SIZE>::Page(Page<SIZE>* prev)
	: 
	dataBegin(reinterpret_cast<char*>(std::malloc(SIZE))),
	dataCurrent(dataBegin),
	previous(prev)
{

}

template<size_t SIZE>
Page<SIZE>::~Page()
{
	std::free(dataBegin);
	if (previous != nullptr)
	{
		previous->~Page<SIZE>();
		std::free(previous);
	}
}

//===== StackAllocatorImpl =====

template<size_t PAGE_SIZE>
StackAllocatorImpl<PAGE_SIZE>::StackAllocatorImpl()
	:
	referenceCounter_(0),
	current_(new(std::malloc(sizeof(page_t))) page_t)
{

}

template<size_t PAGE_SIZE>
StackAllocatorImpl<PAGE_SIZE>::~StackAllocatorImpl()
{
	current_->~page_t();
	std::free(current_);
}

template<size_t PAGE_SIZE>
char* StackAllocatorImpl<PAGE_SIZE>::allocate(size_t count)
{
	if (count > PAGE_SIZE)
	{
		return nullptr;
	}

	if (current_->dataCurrent - current_->dataBegin + count > PAGE_SIZE)
	{
		current_ = new(std::malloc(sizeof(page_t))) page_t(current_);
	}

	char* result = current_->dataCurrent;
	current_->dataCurrent += count;
	return result;
}

template<size_t PAGE_SIZE>
void StackAllocatorImpl<PAGE_SIZE>::AddOwner()
{
	++referenceCounter_;
}

template<size_t PAGE_SIZE>
void StackAllocatorImpl<PAGE_SIZE>::RemoveOwner()
{
	--referenceCounter_;
}

template<size_t PAGE_SIZE>
bool StackAllocatorImpl<PAGE_SIZE>::Alive()
{
	return referenceCounter_ > 0;
}


//===== StackAllocator =====

template<typename T, size_t PAGE_SIZE>
StackAllocator<T, PAGE_SIZE>::StackAllocator()
	: impl_(new(std::malloc(sizeof(impl_t))) impl_t)
{
	impl_->AddOwner();
}


template<typename T, size_t PAGE_SIZE>
template<typename U>
StackAllocator<T, PAGE_SIZE>::StackAllocator(const rebind<U>& that)
	: impl_(that.impl_)
{
	impl_->AddOwner();
}

template<typename T, size_t PAGE_SIZE>
template<typename U>
StackAllocator<T, PAGE_SIZE>::StackAllocator(rebind<U>&& that)
	: impl_(that->impl_)
{
	that->impl_ = nullptr;
}

template<typename T, size_t PAGE_SIZE>
template<typename U>
auto StackAllocator<T, PAGE_SIZE>::operator=(const rebind<U>& that) -> other&
{
	if (&that == this)
		return *this;

	impl_->RemoveOwner();
	if (!impl_->Alive())
	{
		impl_->~impl_t();
		std::free(impl_);
	}

	impl_ = that->impl_;
	impl_->AddOwner();
}

template<typename T, size_t PAGE_SIZE>
template<typename U>
auto StackAllocator<T, PAGE_SIZE>::operator=(rebind<U>&& that) -> other&
{
	if (&that == this)
		return *this;
	
	impl_->RemoveOwner();
	if (!impl_->Alive())
	{
		impl_->~impl_t();
		std::free(impl_);
	}

	impl_ = that->impl_;
	that->impl_ = nullptr;
}

template<typename T, size_t PAGE_SIZE>
auto StackAllocator<T, PAGE_SIZE>::allocate(size_t count) -> pointer
{
	return reinterpret_cast<pointer>(impl_->allocate(count * sizeof(value_type)));
}

template<typename T, size_t PAGE_SIZE>
void StackAllocator<T, PAGE_SIZE>::deallocate(pointer ptr, size_t count)
{

}

template<typename T, size_t PAGE_SIZE>
template<typename... Args>
void StackAllocator<T, PAGE_SIZE>::construct(pointer ptr, Args&&... args)
{
	new(ptr) value_type(std::forward<Args>(args)...);
}

template<typename T, size_t PAGE_SIZE>
void StackAllocator<T, PAGE_SIZE>::destroy(pointer ptr)
{
	ptr->~T();
}

template<typename T, size_t PAGE_SIZE>
StackAllocator<T, PAGE_SIZE>::~StackAllocator()
{
	impl_->RemoveOwner();
	if (!impl_->Alive())
	{
		impl_->~impl_t();
		std::free(impl_);
	}
}
