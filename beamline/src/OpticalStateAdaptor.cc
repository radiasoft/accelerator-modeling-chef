/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OpticalStateAdaptor.cc
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
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
******    
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

//---------------------------------------------------------------------------------
// The OpticalStateAdaptor transforms a particle's canonical state into 
// an optical state. Normally, the particle's canonical state is updated when the
// the  OpticalStateAdaptor goes out of scope, but an update may be forced 
// by calling the sync() method. It is the user's responsibility to make sure
// that the optical state has been correctly opdate *before* the OpticalStateAdaptor
// goes out of scope !  
//-------------------------------------------------------------------------------- 

#include <beamline/OpticalStateAdaptor.h>
#include <mxyzptlk/Mapping.h>
#include <mxyzptlk/TJetVector.h>
#include <iostream>

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OpticalStateAdaptor::OpticalStateAdaptor( Particle& p)
  :  state_is_valid_(true), particle_(p), 
     optical_state_( p.getState().begin(), p.getState().end() )
{

  double npz    = p.get_npz();

  optical_state_[3] /= npz;
  optical_state_[4] /= npz;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

OpticalStateAdaptor::~OpticalStateAdaptor()
{
  sync(); // synchronize canonical state with optical state before exiting.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& OpticalStateAdaptor::operator[](int i) const
{
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double&  OpticalStateAdaptor::operator[](int i) 
{
  state_is_valid_ = false;
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void OpticalStateAdaptor::sync() 
{

  Vector& state     =  particle_.getState();
  
  std::copy( &optical_state_[0],  &optical_state_[0]+state.Dim(), &state[0]); 

  double npz        =   ( (1.0 + optical_state_[5])*(1.0+optical_state_[5] ) ) 
                        / (1.0 + optical_state_[3]*optical_state_[3] +  optical_state_[4]*  optical_state_[4]);

         npz        =   sqrt(npz);

  state[3]   *= npz;
  state[4]   *= npz;

   
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

/// ******* JET **************

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetOpticalStateAdaptor::JetOpticalStateAdaptor( JetParticle& p)
  : state_is_valid_(true), particle_(p), 
    optical_state_( p.getState().begin(), p.getState().end() )
{

  Jet     npz    = p.get_npz();

  optical_state_[3] /= npz;
  optical_state_[4] /= npz;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

JetOpticalStateAdaptor::~JetOpticalStateAdaptor()
{
  sync(); // synchronize canonical state with optical state before exiting.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet const& JetOpticalStateAdaptor::operator[](int i) const
{
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Jet&  JetOpticalStateAdaptor::operator[](int i) 
{
  state_is_valid_ = false;
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void JetOpticalStateAdaptor::sync() 
{

  if (state_is_valid_) return;

  Mapping& state     =  particle_.getState();
  
  std::copy( &optical_state_[0],  &optical_state_[0]+state.Dim(), &state[0]); 

  Jet  npz           =   ( (1.0 + optical_state_[5] )*(1.0 + optical_state_[5]) ) 
                         / (1.0 + optical_state_[3]*optical_state_[3] +  optical_state_[4]*  optical_state_[4]);

         npz         =   sqrt(npz);



  state[3]   *= npz;
  state[4]   *= npz;


  state_is_valid_ = true;
  
  return;

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

