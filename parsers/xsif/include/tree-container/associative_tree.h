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
#include <memory>

// stored_type:				type stored in container
// tree_type:				one of three tree types derived from this base
// container_type:		type of contain to hold children (can be set or multiset)

template< typename stored_type, typename tree_type,  typename container_type >
class associative_tree : public basic_tree<stored_type, tree_type, container_type>
{
protected:
	typedef basic_tree<stored_type, tree_type, container_type > basic_tree_type;
	associative_tree() : basic_tree_type(stored_type()) {}
	explicit associative_tree( const stored_type& stored_obj ) : basic_tree_type(stored_obj) {}
	~associative_tree() {}

public:
	typedef associative_tree<stored_type, tree_type, container_type> associative_tree_type;
	typedef size_t size_type;
	typedef stored_type key_type;
	friend class const_pre_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree<stored_type, tree_type, container_type> >;
	friend class const_post_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree<stored_type, tree_type, container_type> >;
	friend class const_level_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree<stored_type, tree_type, container_type> >;
	// iterator typedefs
	typedef const_associative_iterator<stored_type, tree_type, container_type>										const_iterator;
	typedef associative_iterator<stored_type, tree_type, container_type>											iterator;
	typedef const_pre_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>		const_pre_order_iterator;
	typedef pre_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>			pre_order_iterator;
	typedef const_post_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>		const_post_order_iterator;
	typedef post_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>			post_order_iterator;
	typedef const_level_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>	const_level_order_iterator;
	typedef level_order_descendant_iterator<stored_type, tree_type, container_type, associative_tree_type>			level_order_iterator;
	#if defined(_MSC_VER) && _MSC_VER < 1300
		typedef std::reverse_iterator<iterator, stored_type> reverse_iterator;
		typedef std::reverse_iterator<const_iterator, stored_type> const_reverse_iterator;
	#else
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	#endif

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
		reverse_iterator rend() {return std::reverse_iterator<iterator>(begin()); }
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
	iterator find(const stored_type& element);
	const_iterator find(const stored_type& element) const;
	bool erase(const stored_type& element);
	void erase(iterator it);
	void erase(iterator it_beg, iterator it_end);
	void clear();
	size_type count(const stored_type& element) const;
	iterator lower_bound(const stored_type& element);
	const_iterator lower_bound(const stored_type& element) const;
	iterator upper_bound(const stored_type& element);
	const_iterator upper_bound(const stored_type& element) const;
	std::pair<iterator, iterator> equal_range(const stored_type& element)
	{
		tree_type node_obj(element); // create a search node and search local children
		iterator lower_it(basic_tree_type::children.lower_bound(&node_obj), this);
		iterator upper_it(basic_tree_type::children.upper_bound(&node_obj), this);
		return std::make_pair(lower_it, upper_it);
	}
	std::pair<const_iterator, const_iterator> equal_range(const stored_type& element) const
	{
		tree_type node_obj(element); // create a search node and search local children
		const_iterator lower_it(basic_tree_type::children.lower_bound(&node_obj), this);
		const_iterator upper_it(basic_tree_type::children.upper_bound(&node_obj), this);
		return std::make_pair(lower_it, upper_it);
	}

	// overloaded operations
	bool operator == (const associative_tree_type& rhs) const;
	bool operator < (const associative_tree_type& rhs) const;
	bool operator != (const associative_tree_type& rhs) const { return !(*this == rhs); }
	bool operator > (const associative_tree_type& rhs) const { return rhs < *this; }
	bool operator <= (const associative_tree_type& rhs) const { return !(rhs < *this); }
	bool operator >= (const associative_tree_type& rhs) const { return !(*this < rhs); }

protected:
	iterator insert( const stored_type& element, tree_type* parent ) { return insert(end(), element, parent); }
	iterator insert(const const_iterator pos, const stored_type& element, tree_type* parent);
	iterator insert(const tree_type& tree_obj, tree_type* parent) { return insert(end(), tree_obj, parent); }
	iterator insert(const const_iterator pos, const tree_type& tree_obj, tree_type* parent);
	void set(const tree_type& tree_obj, tree_type* parent);
};

#include "associative_tree.inl"
