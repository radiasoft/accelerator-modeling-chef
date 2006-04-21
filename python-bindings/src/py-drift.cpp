/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-drift.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
#include <boost/python.hpp>
#include <bmlnElmnt.h>
#include <drift.h>

using namespace boost::python;

void wrap_drift () {

class_<drift, bases<bmlnElmnt> >("drift", init<>())
  .def(init<double>() )
  .def(init<char*>() )
  .def(init<char*, double>() );
 
}


#if 0
class drift : public bmlnElmnt
{
public:
  drift();
  drift( double );             // length of drift in meters
  drift( char* );              // name
  drift( char*,                // name
         double );             // length of drift in meters
  drift( bmlnElmntData& );
  drift( const drift& );
  ~drift();

  void accept( BmlVisitor& v ) { v.visitDrift( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitDrift( this ); }

  const char* Type() const;
  virtual bool isType(char* c) { if ( strcmp(c, "drift") != 0 ) return bmlnElmnt::isType(c); else return true; }
  bmlnElmnt* Clone() const { return new drift( *this ); }
} ;

#endif 

