#ifndef BARNACLE_H
#define BARNACLE_H

#include "dlist.h"

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
 ~Barnacle();
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
