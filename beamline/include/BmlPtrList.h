#ifndef BMLPTRLIST_H
#define BMLPTRLIST_H

#include <basic_toolkit/dlist.h>

class BmlPtrList : private dlist
{
  //  A list of pointers to instances of bmlnElmnt objects.
  //  The objects are not owned by the list; their memory
  //    allocation must be handled elsewhere.
  //  The objects may be changed by users of the list.

public:
  BmlPtrList();
  BmlPtrList( BmlPtrList const& );
 ~BmlPtrList();

  BmlPtrList& operator=( BmlPtrList const& );

  void append( bmlnElmnt* );
  void insert( bmlnElmnt* );

  bmlnElmnt* get();   // Iterator; removes elements from list
  bmlnElmnt* operator()( int const& ) const;

  int size() const;
  void clear();   // Preserves the bmlnElmnts
};

#endif //  BMLPTRLIST_H

