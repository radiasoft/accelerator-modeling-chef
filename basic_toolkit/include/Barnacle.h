/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Barnacle.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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
