#include <boost/python.hpp>
#include <Barnacle.h>

using namespace boost::python;

void wrap_barnacle () {

void (BarnacleList::* append_overload_1)(const Barnacle*)                  = &BarnacleList::append;
void (BarnacleList::* append_overload_2)(const char*, const BarnacleData*) = &BarnacleList::append;

void (BarnacleList::* insert_overload_1)(const Barnacle*)                  = &BarnacleList::insert;
void (BarnacleList::* insert_overload_2)(const char*, const BarnacleData*) = &BarnacleList::insert;
  

   // class_<Barnacle>("BarnacleData")


class_<Barnacle>("Barnacle", init<const char*, const BarnacleData*>() )
  .def_readwrite("info",   &Barnacle::info);
 
 

class_<BarnacleList>("BarnacleList")
 .def("append",      append_overload_1)
 .def("append",      append_overload_2)
 .def("insert",      insert_overload_1) 
 .def("insert",      insert_overload_2) 
 .def("eraseFirst",  &BarnacleList::eraseFirst) 
 .def("eraseAll",    &BarnacleList::eraseAll)
 .def("find",        &BarnacleList::find, return_value_policy<reference_existing_object>() )
 .def("lift",        &BarnacleList::lift, return_value_policy<reference_existing_object>() );
 


class_<BarnacleData>("BarnacleData");


}

#if 0

struct BarnacleData {
 BarnacleData() {};
 virtual ~BarnacleData() {};
};

class Barnacle {
private:
 char* id;
 friend class BarnacleList;
public:
 Barnacle( const char*         /* identifier tag  */,
           const BarnacleData* /* the information */ );
 virtual ~Barnacle();
 BarnacleData* info;

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif
};

class BarnacleList {
public:
 BarnacleList();
 ~BarnacleList();
 void            append    ( const Barnacle* );
 void            insert    ( const Barnacle* );
 void            append    ( const char*, const BarnacleData* );
 void            insert    ( const char*, const BarnacleData* );
 char            eraseFirst( const char*     /* identifier */ );
 char            eraseAll  ( const char* = 0 /* identifier */ );
 BarnacleData*   find      ( const char*     /* identifier */,
                                   int   = 1 /* instance   */ ) const;
 Barnacle*       lift      ( const char*     /* identifier */,
                                   int   = 1 /* instance   */ );

#ifdef OBJECT_DEBUG
  static int objectCount;
#endif

private:
 dlist theList;
};

#endif // BARNACLE_H

