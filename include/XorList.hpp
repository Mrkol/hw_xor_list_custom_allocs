#pragma once

#include <memory>
#include <cstddef>
#include <iterator>
#include <functional>


template<typename T, typename T2>
using enable_if_same = 
	typename std::enable_if<
		std::is_same<
			T,
			std::decay_t<T2>>::value,
		int>::type;

template<typename T, typename Allocator = std::allocator<T>>
class XorList
{
	class Node;

	using node_allocator = 
		typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
	using traits =
		typename std::allocator_traits<Allocator>::template rebind_traits<Node>;

public:
	using value_type = T;
	using pointer = T*;
	using reference = T&;

	class iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;

	explicit XorList(const Allocator& alloc = Allocator());

	XorList(const XorList<T, Allocator>& other);
	XorList(XorList<T, Allocator>&& other);

	XorList<T, Allocator>& operator=(const XorList<T, Allocator>& other);
	XorList<T, Allocator>& operator=(XorList<T, Allocator>&& other) noexcept;

	std::size_t size();
	bool empty();

	template<typename T2, enable_if_same<T, T2> = 0>
	void push_front(T2&& value);
	template<typename T2, enable_if_same<T, T2> = 0>
	void push_back(T2&& value);
	void pop_front();
	void pop_back();

	iterator begin();
	iterator end();

	reverse_iterator rbegin();
	reverse_iterator rend();

	template<typename T2, enable_if_same<T, T2> = 0>
	void insert_before(iterator position, T2&& value);
	template<typename T2, enable_if_same<T, T2> = 0>
	void insert_after(iterator position, T2&& value);
	void erase(iterator position);

	~XorList() noexcept;

	class iterator
	{
		friend class XorList<T, Allocator>;
	public:
		using difference_type = std::ptrdiff_t;
		using value_type = XorList<T, Allocator>::value_type;
		using pointer = XorList<T, Allocator>::pointer;
		using reference = XorList<T, Allocator>::reference;
		using iterator_category = std::bidirectional_iterator_tag;

		reference operator*();
		reference operator->();
		iterator& operator++();
		iterator operator++(int);
		iterator& operator--();
		iterator operator--(int);

		bool operator==(const iterator& other);
		bool operator!=(const iterator& other);

	private:
		iterator(Node* current, Node* prev);

		Node* current_;
		Node* prev_;
	};

private:
	struct Node
	{
		template<typename T2, enable_if_same<T, T2> = 0>
		Node(T2&& d, Node* x = nullptr);

		T data;
		Node* xored;
	};

	static Node* xor_(Node* first, Node* second);
	void insertBetween_(Node* first, Node* second, Node* what);
	template<typename T2>
	Node* new_(T2&& value);
	void delete_(Node* node);

	Node* begin_;
	Node* end_;

	std::size_t size_;

	node_allocator allocator_;
};

#include "XorList.tpp"
