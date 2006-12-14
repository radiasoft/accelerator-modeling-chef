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
#include <set>
#include <stack>
#include <queue>
#include <algorithm>
#include <iterator>

/************************************************************************/
/* associative tree child iterators                                     */
/************************************************************************/

// forward declaration
template<typename stored_type, typename tree_type, typename container_type> class basic_tree;
template<typename stored_type> class sequential_tree;
template<typename stored_type, typename tree_type, typename container_type> class associative_tree;

template<typename stored_type, typename tree_type, typename container_type>
class const_associative_iterator : public std::iterator<std::bidirectional_iterator_tag, stored_type>
{
private:
	// typedefs
	friend class basic_tree<stored_type, tree_type, container_type>;
	friend class associative_tree<stored_type, tree_type, container_type>;
public:
	// constructors/destructor
	const_associative_iterator() : pIt_parent(0) {}
	explicit const_associative_iterator(typename container_type::const_iterator it_, const associative_tree<stored_type, tree_type, container_type>* pParent_) : it(it_), pIt_parent(pParent_) {}
	// destructor, copy constructor, and assignment operator will be compiler generated correctly

	// overloaded operators
	bool operator == (const const_associative_iterator& rhs ) const { return pIt_parent == rhs.pIt_parent && it == rhs.it; }
	bool operator != (const const_associative_iterator& rhs ) const { return !(*this == rhs); }

	const stored_type& operator*() const { return  *(*it)->get(); }
	const stored_type* operator->() const { return (*it)->get(); }

	const_associative_iterator& operator ++() { ++it; return *this; }
	const_associative_iterator operator ++(int) { const_associative_iterator old(*this); ++*this; return old; }
	const_associative_iterator& operator --() { --it; return *this; }
	const_associative_iterator operator --(int) { const_associative_iterator old(*this); --*this; return old; }

	// public interface
	const tree_type* node() const { return *it; }

	// data
protected:
	typename container_type::const_iterator it;
	const associative_tree<stored_type, tree_type, container_type>* pIt_parent;
};

template<typename stored_type, typename tree_type, typename container_type>
class associative_iterator : public const_associative_iterator<stored_type, tree_type, container_type>
{
private:
	// typedefs
	using const_associative_iterator<stored_type, tree_type, container_type>::it;
	friend class associative_tree<stored_type, tree_type, container_type>;
public:
	// constructors/destructor
	associative_iterator() : const_associative_iterator<stored_type, tree_type, container_type>() {}
	explicit associative_iterator(typename container_type::iterator it_, associative_tree<stored_type, tree_type, container_type>* pParent_) : const_associative_iterator<stored_type, tree_type, container_type>(it_, pParent_) {}
	// destructor, copy constructor, and assignment operator will be compiler generated correctly

	// overloaded operators
	stored_type& operator*() { return *(*it)->get(); }
	stored_type* operator->() { return (*it)->get(); }
	associative_iterator& operator ++() { ++it;  return *this; }
	associative_iterator operator ++(int) { associative_iterator old(*this); ++*this; return old; }
	associative_iterator& operator --() { --it; return *this; }
	associative_iterator operator --(int) { associative_iterator old(*this); --*this; return old; }

	// public interface
	tree_type* node() const { return *it; }
};

/************************************************************************/
/* sequential tree child iterators                                      */
/************************************************************************/


template<typename stored_type, typename tree_type, typename container_type>
class const_sequential_iterator : public std::iterator<std::random_access_iterator_tag, stored_type>
{
private:
	friend class sequential_tree<stored_type>;
public:
	// typedefs
	#if defined(_MSC_VER) && _MSC_VER < 1300
		typedef std::iterator_traits<std::iterator<std::random_access_iterator_tag, tree_type> >::distance_type difference_type;
	#else
		typedef typename std::iterator_traits<const_sequential_iterator>::difference_type difference_type;
	#endif
	typedef size_t size_type;

	// constructors/destructor
	const_sequential_iterator() : pIt_parent(0) {}
	explicit const_sequential_iterator(typename container_type::const_iterator it_, const tree_type* pParent_) : it(it_), pIt_parent(pParent_) {}
	// destructor, copy constructor, and assignment operator will be compiler generated correctly

	// overloaded operators
	bool operator == (const const_sequential_iterator& rhs ) const { return pIt_parent == rhs.pIt_parent && it == rhs.it; }
	bool operator != (const const_sequential_iterator& rhs ) const { return !(*this == rhs); }
	bool operator < (const const_sequential_iterator& rhs) const { return it < rhs.it; }
	bool operator <= (const const_sequential_iterator& rhs) const { return it <= rhs.it; }
	bool operator > (const const_sequential_iterator& rhs) const { return it > rhs.it; }
	bool operator >= (const const_sequential_iterator& rhs) const { return it >= rhs.it; }

	const stored_type& operator*() const { return  *(*it)->get(); }
	const stored_type* operator->() const { return (*it)->get(); }

	const_sequential_iterator& operator ++() { ++it; return *this; }
	const_sequential_iterator operator ++(int) { const_sequential_iterator old(*this); ++*this; return old; }
	const_sequential_iterator& operator --() { --it; return *this; }
	const_sequential_iterator operator --(int) { const_sequential_iterator old(*this); --*this; return old; }
	const_sequential_iterator& operator +=(size_type n) { it += n; return *this; }
	const_sequential_iterator& operator -=(size_type n) { it -= n; return *this; }
	difference_type operator -(const const_sequential_iterator& rhs) const { return it - rhs.it; }

	// public interface
	const tree_type* node() const { return *it; }

	// data
protected:
	typename container_type::const_iterator it;
	const tree_type* pIt_parent;
};




template<typename stored_type, typename tree_type, typename container_type>
class sequential_iterator : public const_sequential_iterator<stored_type, tree_type, container_type>
{
private:
	friend class sequential_tree<stored_type>;
	using const_sequential_iterator<stored_type, tree_type, container_type>::it;
public:
	// constructors/destructor
	sequential_iterator() : const_sequential_iterator<stored_type, tree_type, container_type>() {}
	explicit sequential_iterator(typename container_type::iterator it_, tree_type* pParent_) : const_sequential_iterator<stored_type, tree_type, container_type>(it_, pParent_) {}
	// destructor, copy constructor, and assignment operator will be compiler generated correctly

	// overloaded operators
	stored_type& operator*() { return *(*it)->get(); }
	stored_type* operator->() const { return (*it)->get(); }
	sequential_iterator& operator ++() { ++it;  return *this; }
	sequential_iterator operator ++(int) { sequential_iterator old(*this); ++*this; return old; }
	sequential_iterator& operator --() { --it; return *this; }
	sequential_iterator operator --(int) { sequential_iterator old(*this); --*this; return old; }

	// public interface
	tree_type* node() const { return *it; }
};






#include "child_iterator.inl"

