/*******************************************************************************
Tree Container Library: Generic container library to store data in tree-like structures.
Copyright (c) 2006  Mitchel Haas

This software is provided 'as-is', without any express or implied warranty. 
In no event will the author be held liable for any damages arising from 
the use of this software.

Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1.	The origin of this software must not be misrepresented; 
you must not claim that you wrote the original software. 
If you use this software in a product, an acknowledgment in the product 
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, 
and must not be misrepresented as being the original software.

3.	The above copyright notice and this permission notice may not be removed 
or altered from any source distribution.

For complete documentation on this library, see http://www.datasoftsolutions.net
Email questions, comments or suggestions to mhaas@datasoftsolutions.net
*******************************************************************************/
#pragma once
#include "basic_tree.h"
#include "child_iterator.h"
#include "descendant_iterator.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

template<typename stored_type>
class sequential_tree : public basic_tree<stored_type, sequential_tree<stored_type>, std::vector<sequential_tree<stored_type>* > >
{
public:
	// typedefs
	typedef sequential_tree<stored_type> tree_type;
	typedef sequential_tree<stored_type> sequential_tree_type;
	typedef std::vector<sequential_tree<stored_type>*> container_type;
	typedef basic_tree<stored_type, tree_type, container_type > basic_tree_type;
	typedef bool (*pPred)(const stored_type& lhs, const stored_type& rhs);
	friend class const_pre_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>;
	friend class const_post_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>;
	friend class const_level_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>;
	// iterator typedefs
	typedef const_sequential_iterator<stored_type, tree_type, container_type>										const_iterator;
	typedef sequential_iterator<stored_type, tree_type, container_type>												iterator;
	typedef const_pre_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>		const_pre_order_iterator;
	typedef pre_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>				pre_order_iterator;
	typedef const_post_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>		const_post_order_iterator;
	typedef post_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>			post_order_iterator;
	typedef const_level_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>		const_level_order_iterator;
	typedef level_order_descendant_iterator<stored_type, tree_type, container_type, sequential_tree_type>			level_order_iterator;
	#if defined(_MSC_VER) && _MSC_VER < 1300
		typedef std::reverse_iterator<iterator, stored_type> reverse_iterator;
		typedef std::reverse_iterator<const_iterator, stored_type> const_reverse_iterator;
	#else
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	#endif
	typedef size_t size_type;
	typedef typename const_sequential_iterator<stored_type, tree_type, container_type>::difference_type difference_type;
	// constructors/destructor
	sequential_tree() : basic_tree_type(stored_type()) {}
	explicit sequential_tree(size_type sz) : basic_tree_type(stored_type()) { basic_tree_type::children.insert(basic_tree_type::children.end(), sz, stored_type()); }
	explicit sequential_tree(size_type sz, const stored_type& element) : basic_tree_type(stored_type()) {basic_tree_type::children.insert(basic_tree_type::children.end(), sz, element); }
	explicit sequential_tree(const stored_type& element) : basic_tree_type(element) {}
	sequential_tree(const sequential_tree<stored_type>& rhs);  // copy constructor
	template<typename iterator_type> sequential_tree(iterator_type it_beg, iterator_type it_end, const stored_type& element = stored_type()) : basic_tree_type(element) { while (it_beg != it_end) insert(*it_beg++); }
	~sequential_tree() {clear(); }

	// assignment operator
	tree_type& operator = (const tree_type& rhs);

	// child iterator accessors
	const_iterator begin() const { return const_iterator(basic_tree_type::children.begin(), this); }
	const_iterator end() const { return const_iterator(basic_tree_type::children.end(), this); }
	iterator begin() { return iterator(basic_tree_type::children.begin(), this); }
	iterator end() { return iterator(basic_tree_type::children.end(), this); }

	// child reverse iterator accessors
	#if defined(_MSC_VER) && _MSC_VER < 1300
		const_reverse_iterator rbegin() const {return std::reverse_iterator<const_iterator, stored_type>(end()); }
		const_reverse_iterator rend() const {return std::reverse_iterator<const_iterator, stored_type>(begin()); }
		reverse_iterator rbegin() {return std::reverse_iterator<iterator, stored_type>(end()); }
		reverse_iterator rend() {return std::reverse_iterator<iterator, stored_type>(begin()); }
	#else
		const_reverse_iterator rbegin() const {return std::reverse_iterator<const_iterator>(end()); }
		const_reverse_iterator rend() const {return std::reverse_iterator<const_iterator>(begin()); }
		reverse_iterator rbegin() {return std::reverse_iterator<iterator>(end()); }
		reverse_iterator rend() { return std::reverse_iterator<iterator>(begin()); }
	#endif

	// descendant iterator accessors
	post_order_iterator post_order_begin() { post_order_iterator it(this); return it; }
	post_order_iterator post_order_end() { iterator it = end(); return post_order_iterator(it, this); }
	const_post_order_iterator post_order_begin() const { const_post_order_iterator it(this); return it; }
	const_post_order_iterator post_order_end() const { const_iterator it = end(); return const_post_order_iterator(it, this); }
	pre_order_iterator pre_order_begin() { iterator it = begin(); return pre_order_iterator(it, this); }
	pre_order_iterator pre_order_end() { iterator it = end(); return pre_order_iterator(it, this); }
	const_pre_order_iterator pre_order_begin() const { const_iterator it = begin(); return const_pre_order_iterator(it, this); }
	const_pre_order_iterator pre_order_end() const { const_iterator it = end(); return const_pre_order_iterator(it, this); }
	level_order_iterator level_order_begin() { iterator it = begin(); return level_order_iterator(it, this); }
	level_order_iterator level_order_end() { iterator it = end(); return level_order_iterator(it, this); }
	const_level_order_iterator level_order_begin() const { const_iterator it = begin(); return const_level_order_iterator(it, this); }
	const_level_order_iterator level_order_end() const { const_iterator it = end(); return const_level_order_iterator(it, this); }

	// public interface
	size_type capacity() const { return basic_tree_type::children.capacity(); }
	void reserve(size_type sz) { basic_tree_type::children.reserve(sz); }
	tree_type& front() { return *basic_tree_type::children.front(); }
	tree_type& back() { return *basic_tree_type::children.back(); }
	const tree_type& front() const { return *basic_tree_type::children.front(); }
	const tree_type& back() const { return *basic_tree_type::children.back(); }
	void push_back(const stored_type& element);
	void pop_back() { iterator it = end(); erase(--it); }

	iterator insert(const stored_type& element);
	iterator insert(const tree_type& tree_obj );
	iterator insert(const_iterator pos, const stored_type& element);
	iterator insert(const_iterator pos, const tree_type& tree_obj);
	void insert(const_iterator pos, size_type num, const stored_type& element);
	template<typename iterator_type> void insert(const_iterator pos, iterator_type it_beg, iterator_type it_end) 
	{ while (it_beg != it_end) { pos = insert(pos, *it_beg++); ++pos; } }
	void set(const stored_type& element) { basic_tree_type::set(element); }
	void set(const tree_type& tree_obj);
	void swap(tree_type& rhs);
	iterator erase(iterator it);
	iterator erase(iterator beg_it, iterator end_it);
	void clear();

	// subscript operators
	tree_type& operator [](size_type index);
	const tree_type& operator [](size_type index) const; 

	// overloaded operations
	bool operator == (const tree_type& rhs) const;
	bool operator < (const tree_type& rhs) const;
	bool operator != (const tree_type& rhs) const { return !(*this == rhs); }
	bool operator > (const tree_type& rhs) const { return rhs < *this; }
	bool operator <= (const tree_type& rhs) const { return !(rhs < *this); }
	bool operator >= (const tree_type& rhs) const { return !(*this < rhs); }

	// children sort operations
	template<typename T> void sort(const T& sort_functor) { std::sort(basic_tree_type::children.begin(), basic_tree_type::children.end(), sort_functor_deref<T>(sort_functor)); }
	void sort() { std::sort(basic_tree_type::children.begin(), basic_tree_type::children.end(), sort_deref()); }
	void sort(const pPred& predicate) { std::sort(basic_tree_type::children.begin(), basic_tree_type::children.end(), sort_pred_deref(predicate)); }

	// descendant sort operations
	template<typename T> void sort_descendants(const T& sort_functor)
	{
		sort(sort_functor);
		post_order_iterator it = post_order_begin(), it_end = post_order_end();
		for ( ; it != it_end; ++it )
		{
			it.node()->sort(sort_functor);
		}
	}
	void sort_descendants(); 
	void sort_descendants(const pPred& predicate);

	// overloaded iterator arithmetic operators
	friend const_iterator operator +(const const_iterator& lhs, size_type n) 
	{ const_iterator temp(lhs); temp += n; return temp; }

	friend const_iterator operator +(size_type n, const const_iterator& rhs)
	{ const_iterator temp(rhs); temp += n; return temp; }

	friend const_iterator operator -(const const_iterator& lhs, size_type n)
	{ const_iterator temp(lhs); temp -= n; return temp; }

	friend iterator operator +(const iterator& lhs, size_type n)
	{ iterator temp(lhs); temp += n; return temp; }

	friend iterator operator +(size_type n, const iterator& rhs)
	{ iterator temp(rhs); temp += n; return temp; }

	friend iterator operator -(const iterator& lhs, size_type n)
	{ iterator temp(lhs); temp -= n; return temp; }



private:
	// sort() dereference functor
	struct sort_deref
	{
		bool operator() (const tree_type* lhs, const tree_type* rhs)
		{
			return *lhs->get() < *rhs->get();
		}
	};

	// sort(predicate) dereference functor
	struct sort_pred_deref
	{
		explicit sort_pred_deref(const pPred& predicate_) : predicate(predicate_) {}
		bool operator() (const tree_type* lhs, const tree_type* rhs)
		{
			return predicate(*lhs->get(), *rhs->get());
		}

		pPred predicate;
	};

	// sort<T>() dereference functor
	template<typename T>
	struct sort_functor_deref 
	{
		explicit sort_functor_deref(const T& sort_functor_) : sort_functor(sort_functor_) {}
		bool operator() (const tree_type* lhs, const tree_type* rhs) const
		{
			return sort_functor(*lhs->get(), *rhs->get());
		}
		sort_functor_deref& operator = (const sort_functor_deref& rhs) { sort_functor = rhs->sort_functor; return *this; }
		const T& sort_functor;
	};
};




#include "sequential_tree.inl"
