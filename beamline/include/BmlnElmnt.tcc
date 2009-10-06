/*******************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************
******                                                                    ******
******    BEAMLINE: C++ objects for design and analysis                   ******
******              of beamlines, storage rings, and                      ******
******              synchrotrons.                                         ******
******                                                                    ******
******    File:      BmlnElmnt.tcc                                        ******
******                                                                    ******
******    Copyright (c) Universities Research Association, Inc./Fermilab  ******
******                  All Rights Reserved                               ******
******                                                                    ******
******    Usage, modification, and redistribution are subject to terms    ******
******    of the License supplied with this software.                     ******
******                                                                    ******
******    Software and documentation created under                        ******
******    U.S. Department of Energy Contract No. DE-AC02-76CH03000.       ******
******    The U.S. Government retains a world-wide non-exclusive,         ******
******    royalty-free license to publish or reproduce documentation      ******
******    and software for U.S. Government purposes. This software        ******
******    is protected under the U.S. and Foreign Copyright Laws.         ******
******                                                                    ******
******    Author: Jean-Francois Ostiguy                                   ******
******            ostiguy@fnal.gov                                        ******
******                                                                    ******
******
******  REVISION HISTORY                                                  ******
******  ----------------                                                  ******
******                                                                    ******
******
********************************************************************************
********************************************************************************
********************************************************************************
********************************************************************************/

#include <beamline/PhaseSpaceIndexing.h>
#include <beamline/BasePropagator.h>
#include <beamline/Alignment.h>


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void BmlnElmnt::enterLocalFrame( Particle_t& p ) const
{
  typedef PhaseSpaceIndexing::index index;

  static index const i_x   = PhaseSpaceIndexing::i_x;
  static index const i_y   = PhaseSpaceIndexing::i_y;
  static index const i_cdt = PhaseSpaceIndexing::i_cdt;
  static index const i_npx = PhaseSpaceIndexing::i_npx;
  static index const i_npy = PhaseSpaceIndexing::i_npy;
  static index const i_ndp = PhaseSpaceIndexing::i_ndp;
 
  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const cs = align_->cos_roll();
  double const sn = align_->sin_roll();

  State_t& state = p.state();

   //----------------------------
   // apply roll 
   //----------------------------

  if( align_->roll() != 0.0) {

    Component_t temp  = state[i_x] * cs + state[i_y] * sn;
    state[i_y]        = state[i_y] * cs - state[i_x] * sn;
    state[i_x]        = temp;

    temp           = state[i_npx] * cs + state[i_npy] * sn;
    state[i_npy]   = state[i_npy] * cs - state[i_npx] * sn;
    state[i_npx]   = temp;
  }

   //----------------------------
   // apply pitch and offsets ... 
   //----------------------------

   double const   pitch   = align_->pitch();
   Component_t    npz    =  p.npz(); 

 
   state[i_x] -=  align_->xOffset();
   state[i_y] -= (align_->yOffset() - 0.5*pitch*length_);

   state[i_npy]  -=  pitch*npz;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

template <typename Particle_t>
void BmlnElmnt::leaveLocalFrame( Particle_t& p ) const
{

  typedef PhaseSpaceIndexing::index index;

  static index const i_x   = PhaseSpaceIndexing::i_x;
  static index const i_y   = PhaseSpaceIndexing::i_y;
  static index const i_cdt = PhaseSpaceIndexing::i_cdt;
  static index const i_npx = PhaseSpaceIndexing::i_npx;
  static index const i_npy = PhaseSpaceIndexing::i_npy;
  static index const i_ndp = PhaseSpaceIndexing::i_ndp;

  typedef typename PropagatorTraits<Particle_t>::State_t       State_t;
  typedef typename PropagatorTraits<Particle_t>::Component_t   Component_t;

  double const cs = align_->cos_roll();
  double const sn = align_->sin_roll();

  State_t& state = p.state();
  double         pitch = align_->pitch();
  Component_t    npz   =  p.npz(); 

  //----------------------------
  // undo pitch and offsets ... 
  //----------------------------

  state[i_x] +=  align_->xOffset();
  state[i_y] += (align_->yOffset() + 0.5*pitch*length_);

  state[i_npy]  +=  pitch*npz;

  //----------------------------
  // undo roll  
  //----------------------------

  if( align_->roll() != 0.0) {
    Component_t   temp   = state[i_x] * cs - state[i_y] * sn;
    state[i_y]           = state[i_y] * cs + state[i_x] * sn;
    state[i_x]           = temp;

        temp        = state[i_npx] * cs - state[i_npy] * sn;
    state[i_npy]    = state[i_npy] * cs + state[i_npx] * sn;
    state[i_npx]    = temp;
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
