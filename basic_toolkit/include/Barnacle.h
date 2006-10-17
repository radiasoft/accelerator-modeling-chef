/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Barnacle.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./Fermilab    
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
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******               
******                                                                
******                                                                
**************************************************************************
*************************************************************************/
#ifndef BARNACLE_H
#define BARNACLE_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/dlist.h>

struct DLLEXPORT BarnacleData {
 bool valid;
 BarnacleData() : valid(false) {};
 virtual ~BarnacleData() {};
};

class DLLEXPORT Barnacle {
private:
 char* id;
 friend class BarnacleList;
public:
 Barnacle( const char*         /* identifier tag  */,
           const BarnacleData* /* the information */ );
 virtual ~Barnacle();
 BarnacleData* info;

};

class DLLEXPORT BarnacleList {

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

private:
 dlist theList;
};

#endif // BARNACLE_H
