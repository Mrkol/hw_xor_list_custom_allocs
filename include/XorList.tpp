#include "XorList.hpp"


template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(
		const Allocator& alloc)
	: begin_(nullptr), end_(nullptr), size_(0),
	allocator_(alloc)
{

}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(const XorList<T, Allocator>& other)
	: begin_(nullptr), end_(nullptr), size_(0),
	allocator_(other.allocator_)
{
	for (auto i = other.begin(); i != other.end(); ++i)
	{
		push_back(*i);
	}
}

template<typename T, typename Allocator>
XorList<T, Allocator>::XorList(
		XorList<T, Allocator>&& other)
	: begin_(other.begin_), end_(other.end_), size_(other.size_),
	allocator_(std::move(other.allocator_))
{
	other.begin_ = nullptr;
	other.end_ = nullptr;
}

template<typename T, typename Allocator>
XorList<T, Allocator>& XorList<T, Allocator>::operator=(
	const XorList<T, Allocator>& other)
{
	if (&other == this)
		return *this;

	while (!empty())
	{
		pop_back();
	}

	allocator_ = other.allocator_;

	for (auto i = other.begin(); i != other.end(); ++i)
	{
		push_back(*i);
	}
}

template<typename T, typename Allocator>
XorList<T, Allocator>& XorList<T, Allocator>::operator=(
	XorList<T, Allocator>&& other) noexcept
{
	if (&other == this)
		return *this;

	while (!empty())
	{
		pop_back();
	}

	allocator_ = std::move(other.allocator_);

	begin_ = other.begin_;
	end_ = other.end_;

	other.begin_ = nullptr;
	other.end_ = nullptr;
}

template<typename T, typename Allocator>
std::size_t XorList<T, Allocator>::size()
{
	return size_;
}

template<typename T, typename Allocator>
bool XorList<T, Allocator>::empty()
{
	return size_ == 0;
}

template<typename T, typename Allocator>
template<typename T2, enable_if_same<T, T2>>
void XorList<T, Allocator>::push_front(T2&& value)
{
	Node* node = new_(std::forward<T2>(value));
	insertBetween_(nullptr, begin_, node);
	begin_ = node;
	if (end_ == nullptr) end_ = node;
}

template<typename T, typename Allocator>
template<typename T2, enable_if_same<T, T2>>
void XorList<T, Allocator>::push_back(T2&& value)
{
	std::swap(begin_, end_);
	push_front<T2>(std::forward<T2>(value));
	std::swap(begin_, end_);
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_front()
{
	Node* node = begin_->xored;
	insertBetween_(begin_, node, nullptr);
	if (end_ == begin_) end_ = nullptr;
	delete_(begin_);
	begin_ = node;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::pop_back()
{
	std::swap(begin_, end_);
	pop_front();
	std::swap(begin_, end_);
}

template<typename T, typename Allocator>
template<typename T2, enable_if_same<T, T2>>
void XorList<T, Allocator>::insert_before(iterator position, T2&& value)
{
	if (position.current_ == begin_)
		push_front(std::forward<T2>(value));
	if (position.prev_ == end_)
		push_back(std::forward<T2>(value));
	else
		insertBetween_(position.prev_, position.current_, new_(value));
}

template<typename T, typename Allocator>
template<typename T2, enable_if_same<T, T2>>
void XorList<T, Allocator>::insert_after(iterator position, T2&& value)
{
	insert_before(++position, value);
}


template<typename T, typename Allocator>
void XorList<T, Allocator>::erase(iterator position)
{
	if (position.current_ == begin_)
		pop_front();
	else if (position.current_ == end_)
		pop_back();
	else
	{
		iterator next = position;
		++next;

		if (position.prev_ != nullptr)
			position.prev_->xored = 
				xor_(xor_(position.prev_->xored, position.current_), next.current_);
		
		if (next.current_ != nullptr)
			next.current_->xored = 
				xor_(xor_(next.current_->xored, position.current_), position.prev_);

		delete_(position.current_);
	}
}

template<typename T, typename Allocator>
template<typename T2>
auto XorList<T, Allocator>::new_(T2&& value) -> Node*
{
	++size_;
	Node* node = traits::allocate(allocator_, 1);
	traits::construct(node, std::forward<T2>(value));
	return node;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::delete_(Node* node)
{
	traits::deallocate(allocator_, node, 1);
	traits::destroy(node);
	--size_;
}

template<typename T, typename Allocator>
void XorList<T, Allocator>::insertBetween_(Node* first, Node* second, Node* what)
{
	if (first != nullptr)
		first->xored = xor_(xor_(first != nullptr ? first->xored : nullptr, second), what);

	if (second != nullptr)
		second->xored = xor_(xor_(first, second != nullptr ? second->xored : nullptr), what);

	if (what != nullptr)
		what->xored = xor_(first, second);
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::begin() -> iterator
{
	return iterator(begin_, nullptr);
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::end() -> iterator
{
	return iterator(nullptr, end_);
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::rbegin() -> reverse_iterator
{
	return reverse_iterator(end());
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::rend() -> reverse_iterator
{
	return reverse_iterator(begin());
}

template<typename T, typename Allocator>
XorList<T, Allocator>::~XorList() noexcept
{
	while (!empty())
	{
		pop_back();
	}
}

template<typename T, typename Allocator>
template<typename T2, enable_if_same<T, T2>>
XorList<T, Allocator>::Node::Node(T2&& d, Node* x)
	: data(std::forward<T2>(d)), xored(x)
{

}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator*() -> reference
{
	return current_->data;
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator->() -> reference
{
	return current_->data;
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator++() -> iterator&
{
	auto tmp = current_;
	current_ = xor_(current_->xored, prev_);
	prev_ = tmp;
	return *this;
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator++(int) -> iterator
{
	auto copy = *this;
	++*this;
	return copy;
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator--() -> iterator&
{
	auto tmp = prev_;
	prev_ = xor_(prev_->xored, current_);
	current_ = tmp;
	return *this;
}

template<typename T, typename Allocator>
auto XorList<T, Allocator>::iterator::operator--(int) -> iterator
{
	auto copy = *this;
	--*this;
	return copy;
}

template<typename T, typename Allocator>
bool XorList<T, Allocator>::iterator::operator==(const iterator& other)
{
	return current_ == other.current_;
}

template<typename T, typename Allocator>
bool XorList<T, Allocator>::iterator::operator!=(const iterator& other)
{
	return !this->operator==(other);
}


template<typename T, typename Allocator>
XorList<T, Allocator>::iterator::iterator(Node* current, Node* prev)
	: current_(current), prev_(prev)
{

}

template<typename T, typename Allocator>
typename XorList<T, Allocator>::Node*
	XorList<T, Allocator>::xor_(Node* first, Node* second)
{
	return reinterpret_cast<Node*>(
		reinterpret_cast<std::uintptr_t>(first) ^
		reinterpret_cast<std::uintptr_t>(second));
}
