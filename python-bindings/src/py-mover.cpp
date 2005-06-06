/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-mover.cpp
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

#include <mover.h>


void wrap_mover () {
  

using namespace boost::python;


class_<mover>("mover")
  .def(init<const char*>())
  .def( "switchOn",   &mover::switchOn)
  .def( "switchOff",  &mover::switchOff);
 
}

#if 0

class mover : public circuit {
protected:
  alignmentData align;
public:
  mover();
  ~mover();
  mover( const char* );
  mover( bmlnElmnt* );
  mover( const char*, bmlnElmnt* );
  void append( bmlnElmnt* );
  void switchOn();
  void switchOff();
  void set( void* );
  void get( void* );

};

#endif // MOVER_H
