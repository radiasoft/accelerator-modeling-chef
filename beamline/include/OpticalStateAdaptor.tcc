/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      OpticalStateAdaptor.tcc
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

template <typename Particle_t>
OpticalStateAdaptor<Particle_t>::OpticalStateAdaptor( Particle_t& p)
  :  state_is_valid_(true), particle_(p), 
     optical_state_( p.State().begin(), p.State().end() )
{

  Component_t npz    = p.get_npz();

  optical_state_[3] /= npz;
  optical_state_[4] /= npz;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
OpticalStateAdaptor<Particle_t>::~OpticalStateAdaptor()
{
  sync(); // synchronize canonical state with optical state before exiting.
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename OpticalStateAdaptor<Particle_t>::Component_t const& OpticalStateAdaptor<Particle_t>::operator[](int i) const
{
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
typename OpticalStateAdaptor<Particle_t>::Component_t&  OpticalStateAdaptor<Particle_t>::operator[](int i) 
{
  state_is_valid_ = false;
  return optical_state_[i];
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void OpticalStateAdaptor<Particle_t>::sync() 
{

  State_t& state     =  particle_.State();
  
  std::copy( &optical_state_[0],  &optical_state_[0]+state.Dim(), &state[0]); 

  Component_t npz     =   ( (1.0 + optical_state_[5])*(1.0+optical_state_[5] ) ) 
                        / (1.0 + optical_state_[3]*optical_state_[3] +  optical_state_[4]*  optical_state_[4]);

         npz        =   sqrt(npz);

  state[3]   *= npz;
  state[4]   *= npz;

   
}
