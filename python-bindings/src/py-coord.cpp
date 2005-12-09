/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-coord.cpp
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

#include <Jet.h>
#include <JetC.h>

using namespace boost::python;



// Coord is a special type of Jet. It cannot be copied. 
 
void wrap_mxyzptlk_coord() {

  class_<coord,  bases<Jet>,  boost::noncopyable> ("coord", init<double>() );
  class_<coordC, bases<JetC>, boost::noncopyable> ("coordC", init<std::complex<double> >() );

}
