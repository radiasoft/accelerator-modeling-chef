/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-basictoolkit-module.cpp
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

using namespace boost::python;

extern void wrap_barnacle();
extern void wrap_vector();
extern void wrap_matrix();
extern void wrap_matrixc();
extern void wrap_slist();

BOOST_PYTHON_MODULE( basic_toolkit )
{

wrap_barnacle();
wrap_vector(); 
wrap_matrix(); 
wrap_matrixc(); 
wrap_slist();

}

