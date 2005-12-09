/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-mxyzptlk-module.cpp
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

extern void wrap_mxyzptlk_jetenv();
extern void wrap_mxyzptlk_jetcenv();
extern void wrap_mxyzptlk_jet();
extern void wrap_mxyzptlk_jetc();
extern void wrap_mxyzptlk_jetvector();
extern void wrap_mxyzptlk_jetvectorc();
extern void wrap_mxyzptlk_mapping();
extern void wrap_mxyzptlk_mappingc();
extern void wrap_mxyzptlk_coord();

BOOST_PYTHON_MODULE(mxyzptlk)
{

  wrap_mxyzptlk_jetenv();
  wrap_mxyzptlk_jetcenv();
  wrap_mxyzptlk_jet();
  wrap_mxyzptlk_jetc();
  wrap_mxyzptlk_jetvector();
  wrap_mxyzptlk_jetvectorc();
  wrap_mxyzptlk_coord();
  wrap_mxyzptlk_mapping();
  wrap_mxyzptlk_mappingc();

}    


