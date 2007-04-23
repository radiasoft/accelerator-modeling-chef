/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      TWake.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******                                                                
******
**************************************************************************
*************************************************************************/

#ifndef TWAKE_H
#define TWAKE_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class TWake;

typedef boost::shared_ptr<TWake>        TWakePtr;
typedef boost::shared_ptr<TWake const>  ConstTWakePtr;


class DLLEXPORT TWake : public bmlnElmnt {

  public:

  TWake( char const* name = "");      
  TWake( TWake const&         );

  TWake* Clone() const { return new TWake( *this ); }

  TWake& operator=( TWake const& rhs);

 ~TWake();

  void localPropagate( ParticleBunch& x );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type()     const;
  bool        isMagnet() const;

} ;

#endif // TWAKE_H
