#ifndef CNTR_LIST_H
#define	CNTR_LIST_H

#include <cstdlib>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

#include <iostream>

namespace cntr {

	template<typename T, typename Allocator = std::allocator<T>>
	class list {
		public:
			template<typename Ref>
			class iter;
			template<typename Ref>
			class riter;
			
			typedef T value_type;
			typedef Allocator allocator_type;
			typedef std::size_t size_type;
			typedef std::ptrdiff_t difference_type;
			typedef value_type& reference;
			typedef const value_type& const_reference;
			typedef typename std::allocator_traits<Allocator>::pointer pointer;
			typedef typename std::allocator_traits<Allocator>::const_pointer const_pointer;
			
			typedef iter<reference> iterator;
			typedef iter<const_reference> const_iterator;
			
			typedef riter<const_reference> reverse_iterator;
			typedef riter<const_reference> const_reverse_iterator;
			
			explicit list(const Allocator& alloc = Allocator())
			  : head_(value_type()), tail_(&head_), size_(), alloc_(alloc), node_alloc_()
			{ }
			
			list(size_type count, const T& value, const Allocator& alloc = Allocator())
			  : list(alloc)
			{
				dispatch_ambiguous_constructor(count, value, std::true_type());
			}
			
			explicit list(size_type count)
			  : list(count, value_type())
			{ }
			
			template <typename InputIt>
			list(InputIt first, InputIt last, const Allocator& alloc = Allocator())
			  : list(alloc)
			{
				// In the fairly common case that the list(count, value) constructor
				// is used with a signed integer for count, there's an ambiguity.
				// In particular, list(count, value) expects an unsigned count.
				// This means that this constructor is used instead of list().
				// For this reason, this ctor must be disabled when the InputIt
				// is numeric and not an actual iterator.
				
				using integral = typename std::is_integral<InputIt>::type;
				
				dispatch_ambiguous_constructor(first, last, integral());
				
			}
			
			list(const list& other)
			  : list(other.begin(), other.end(), other.alloc_)
			{ }
			
			list(const list& other, const Allocator& alloc)
			  : list(other.begin(), other.end(), alloc)
			{ }
			
			list(list&& other)
			  : head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_), 
				alloc_(std::move(other.alloc_)), node_alloc_(std::move(other.node_alloc_))
			{
				// TODO: this is not required. Decide if the tiny performance hit
				// is worth the easier testing/debugging.
				other.tail_ = nullptr;
				other.size_ = 0;
			}
			
			list(list&& other, const Allocator& alloc);
			
			list(std::initializer_list<T> init, const Allocator& alloc = Allocator())
			  : list(std::begin(init), std::end(init), alloc)
			{ }
			
			list& operator=(const list& other)
			{
				swap(*this, list(other));
				return *this;
			}
			
			list& operator=(list&& other)
			{
				std::swap(*this, list(std::forward(other)));
				return *this;
			}
			
			list& operator=(std::initializer_list<T> ilist)
			{
				*this = list(ilist);
				return *this;
			}
			
			bool empty() const
			{ return size_ == 0; }
			
			size_type size() const
			{ return size_; }
			
			void push_back(const T& value)
			{
				tail_->setNext(Node::make_node(value, tail_, nullptr));
				tail_ = tail_->next();
				++size_;
			}
			
			void push_back(T&& value)
			{
				tail_->setNext(Node::make_node(std::forward<T>(value), tail_, nullptr));
				tail_ = tail_->next();
				++size_;
			}
			
			template<class... Args>
			void emplace_back( Args&&... args );
			
			void clear()
			{
				head_->setNext(nullptr);
				tail_ = nullptr;
				size_ = 0;
			}
			
			void pop_back()
			{
				Node* tail = tail_;
				Node* tailPrev = tail->previous();
				tailPrev->setNext(nullptr);
				tail_ = tailPrev;
				--size_;
			}
			
			void pop_front()
			{
				head_.popFront();
				--size_;
			}
			
			reference front()
			{ return head_.next()->value(); }
			
			constexpr const_reference front() const
			{ return head_.next()->value(); }
			
			reference back()
			{ return tail_->value(); }
			
			constexpr reference back() const
			{ return tail_->value(); }
			
			friend void swap(list& first, list& second) noexcept
			{
				using std::swap;
				swap(first.head_, second.head_);
				swap(first.tail_, second.tail_);
				swap(first.size_, second.size_);
				swap(first.alloc_, second.alloc_);
				swap(first.node_alloc_, second.node_alloc_);
			}
			
			iterator begin()
			{ return iterator{&head_}; }
			
			const_iterator begin() const
			{  return const_iterator{&head_}; }
			
			const_iterator cbegin() const
			{ return begin(); }
			
			iterator end()
			{ return iterator{tail_}; }
			
			const_iterator end() const
			{ return const_iterator{tail_}; }
			
			const_iterator cend() const
			{ return const_iterator{tail_}; }
			
			reverse_iterator rbegin()
			{ return reverse_iterator{tail_}; }
			
			const_reverse_iterator rbegin() const
			{ return const_reverse_iterator{tail_}; }
			
			reverse_iterator rend()
			{ return reverse_iterator{&head_}; }
			
			const_reverse_iterator rend() const
			{ return const_reverse_iterator{&head_}; }
			
			allocator_type get_allocator() const
			{
				return alloc_;
			}
			
		private:
			
			class Node {
			public:
				
				typedef typename std::allocator_traits<allocator_type>::template rebind_alloc<Node> node_allocator;
				
				Node(Node&& n)
				  : value_(std::move(n.value_)), previous_(n.previous_), next_(n.next_)
				{
					n.previous_ = nullptr;
					n.next_ = nullptr;
				}
				
				Node(const T& value, Node* previous = nullptr, Node* next = nullptr)
				  : value_(value), previous_(previous), next_(next)
				{ }
				
				void setPrevious(Node* previous) noexcept
				{
					previous_ = previous;
				}
				
				void setNext(Node* next)
				{
					dispose_node(next_);
					next_ = next;
				}
				
				Node* previous() const noexcept
				{ return previous_; }
				
				Node* next() const noexcept
				{ return next_; }
				
				reference value() noexcept
				{ return value_; }
				
				const_reference value() const noexcept
				{ return value_; }
				
				void popFront()
				{
					// The node after this one (next_) must be removed from the chain.
					// In other words, a -> b -> c must become a -> c.
					
					Node* b = next_;
					Node* c = next_->next_;
					
					// TODO: can delete throw an exception? If so, this is unsafe.
					b->next_ = nullptr;
					dispose_node(b);
					
					next_ = c;
					
				}
				
				~Node()
				{
					dispose_node(next_);
				}
				
				friend void swap(Node& first, Node& second) noexcept
				{
					using std::swap;
					swap(first.value_, second.value_);
					swap(first.previous_, second.previous_);
					swap(first.next_, second.next_);
				}
				
				static node_allocator allocator()
				{ return node_allocator(); }
				
				static Node* make_node(const value_type& value, Node* previous, Node* next)
				{
					auto alloc = node_allocator();
					Node* n = alloc.allocate(1);
					try {
						alloc.construct(n, value, previous, next);
						return n;
					} catch (...) {
						alloc.deallocate(n, 1);
						throw;
					}
				}
				
				static void dispose_node(Node* n)
				{
					if (!n) { return; }
					auto alloc = node_allocator();
					alloc.destroy(n);
					alloc.deallocate(n, 1);
				}
				
			private:
				T value_;
				Node* previous_;
				Node* next_;
			};
			
			Node head_;
			Node* tail_;
			size_type size_;
			allocator_type alloc_;
			
			using node_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<Node>;
			node_allocator_type node_alloc_;
			
			void dispatch_ambiguous_constructor(size_type count, const T& value, std::true_type)
			{
				for (size_type c = 0; c < count; ++c) {
					push_back(value);
				}
			}
			
			template<typename Iter>
			void dispatch_ambiguous_constructor(Iter first, const Iter& last, std::false_type)
			{
				for (; first != last; ++first) {
					push_back(*first);
				}
			}
			
		public:
			
			template<typename Ref>
			class iter : public std::iterator<std::forward_iterator_tag, T, difference_type, Ref> {
			public:
				
				explicit iter(const Node* node = nullptr)
				  : node_(const_cast<Node*>(node))
				{ }
				  
				iter(const iterator& it)
				  : node_(it.node())
				{ }
				
				Ref operator*()
				{ return node_->next()->value(); }
				
				bool operator==(const iter& other)
				{ return node_ == other.node_; }
				
				bool operator!=(const iter& other)
				{ return !(*this == other); }
				
				iter& operator++()
				{
					node_ = node_->next();
					return *this;
				}
				
				iter operator++(int)
				{
					auto cpy = *this;
					++*this;
					return cpy;
				}
				
				iter& operator--()
				{
					node_ = node_->previous();
					return *this;
				}
				
				iter& operator--(int)
				{
					auto cpy = *this;
					--*this;
					return cpy;
				}
				
				Node* node() const
				{ return node_; }
				
			protected:
				Node* node_;
			};
			
			template<typename Ref>
			class riter : public iter<Ref> {
			public:
				
				// Would be nice to use inheriting constructors, 
				// but my compiler does not support them
				explicit riter(const Node* node = nullptr)
				  : iter<Ref>(node)
				{ }
				  
				Ref operator*()
				{ return this->node_->value(); }
				
				riter& operator++()
				{
					this->node_ = this->node_->previous();
					return *this;
				}
				
				riter& operator--()
				{
					this->node_ = this->node_->next();
					return *this;
				}
			
			};
			
	};

}

#endif
