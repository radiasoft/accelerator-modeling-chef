/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                                                
******  File:      DispersionSage.cc
******  Version:   1.0
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <physics_toolkit/ClosedOrbitSage.h>
#include <physics_toolkit/DispersionSage.h>

extern int filterTransverseTunes( /* const */ MatrixD&, Vector& );

// ... Globals:
const int DispersionSage::DONE             = 0;
const int DispersionSage::TOO_MANY_VECTORS = 7;
const int DispersionSage::IMPOSSIBILITY    = 111;


DispersionSage::Info::Info()
{
  arcLength = -1.0;
  closedOrbit.hor = 0.0;
  closedOrbit.ver = 0.0;
  dispersion.hor = 0.0;
  dispersion.ver = 0.0;
  dPrime.hor = 0.0;
  dPrime.ver = 0.0;
}

DispersionSage::Info::Info( const Info& x )
{
  memcpy((void*) this, (const void*) &x, sizeof(DispersionSage::Info));
}

DispersionSage::Info& DispersionSage::Info::operator=( const Info& x )
{
  if( this != &x ) {
    memcpy((void*) this, (const void*) &x, sizeof(DispersionSage::Info));
  }
  return *this;
}



DispersionSage::GlobalInfo::GlobalInfo()
{
  tune.hor = 0.0;
  tune.ver = 0.0;
  chromaticity.hor = 0.0;
  chromaticity.ver = 0.0;
}

DispersionSage::GlobalInfo::GlobalInfo( const GlobalInfo& x )
{
  memcpy((void*) this, (const void*) &x, sizeof(DispersionSage::GlobalInfo));
}

DispersionSage::GlobalInfo& DispersionSage::GlobalInfo::operator=( const GlobalInfo& x )
{
  if( this != &x ) {
    memcpy((void*) this, (const void*) &x, sizeof(DispersionSage::GlobalInfo));
  }
  return *this;
}



DispersionSage::Options::Options()
: onClosedOrbit(false)
{
}


DispersionSage::Options::Options( const Options& x )
: onClosedOrbit(x.onClosedOrbit)
{
}


DispersionSage::Options& DispersionSage::Options::operator=( const Options& x )
{
  if( this != &x ) {
    memcpy((void*) this, (const void*) &x, sizeof(DispersionSage::Options));
  }
  return *this;
}


const DispersionSage::Options DispersionSage::defaultFlags;


DispersionSage::DispersionSage( const beamline* x, bool doClone )
: Sage( x, doClone ), 
  _dpp( 0.00005 ), 
  _ignoreErrors( false ),
  _calcs((DispersionSage::Info*) 0),
  _n(0)
{
  // REMOVE: _finishConstructor();
}

DispersionSage::DispersionSage( const beamline& x, bool doClone )
: Sage( &x, doClone ), 
  _dpp( 0.00005 ), 
  _ignoreErrors( false ),
  _calcs((DispersionSage::Info*) 0),
  _n(0)
{
  // REMOVE: _finishConstructor();
}


void DispersionSage::_finishConstructor()
{
  _n = _arrayPtr->size();
  if( 0 < _n ) { _calcs = new Info[_n]; }
}

DispersionSage::~DispersionSage()
{
  _deleteCalcs();
}

void DispersionSage::_deleteCalcs()
{
  _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );

  DeepBeamlineIterator dbi( *_myBeamlinePtr );
  bmlnElmnt* be = 0;
  while((  be = dbi++  )) {
    be->dataHook.eraseAll( "DispersionSage" );
  }

  // REMOVE: if( _calcs ) { 
  // REMOVE:   for( int i = 0; i < _n; i++ )
  // REMOVE:   { if( 0 != _calcs[i] ) {delete _calcs[i];} }
  // REMOVE:   delete [] _calcs;
  // REMOVE:   _calcs = 0;
  // REMOVE:   _n = 0;
  // REMOVE: }
  if( _calcs ) { 
    delete [] _calcs;
    _calcs = 0;
    _n = 0;
  }
}


int DispersionSage::doCalc( JetParticle* p_jp, beamline::Criterion& crit )
{
  return ( this->fullCalc( p_jp, crit ) );
}


int DispersionSage::fullCalc( JetParticle* p_jp, beamline::Criterion& )
{
  // 
  // This is copied and modified from 
  // int LattFuncSage::NewDisp_Calc( const JetParticle* arg_jp, 
  //                                 bool onClosedOrbit )
  // 

  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::fullCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  // Create the _calcs array
  _deleteCalcs();
  _finishConstructor();

  // Preserve the current Jet environment
  Jet__environment_ptr storedEnv = Jet__environment::getLastEnv();

  int ret = 0;
  Particle* firstParticle = 0;
  Particle* secondParticle = 0;
  MatrixD   firstJacobian;
  MatrixD   secondJacobian;

  short int i_x   =  Particle::_x();
  short int i_y   =  Particle::_y();
  short int i_z   =  Particle::_cdt();
  short int i_px  =  Particle::_xp();
  short int i_py  =  Particle::_yp();
  short int i_dpp =  Particle::_dpop();

  double energy;
  double mass;
  double momentum;
  double lng = 0.0;


  // Preliminary steps ...
  ClosedOrbitSage clsg( _myBeamlinePtr );
  if( _verbose ) { clsg.set_verbose(); }


  // If the argument is not already declared
  //   to be on the closed orbit, then use a
  //   ClosedOrbitSage to rebuild it.
  JetParticle* local_jp = 0;
  if( !(this->flags.onClosedOrbit) ) { 
    clsg.setForcedCalc(); 
    local_jp = p_jp->Clone();
    ret = clsg.findClosedOrbit( local_jp );
    clsg.unsetForcedCalc();

    // Review the solution
    if( 0 == ret ) {
      if( _verbose ) {
        *_outputStreamPtr << "DispersionSage -- Closed orbit successfully calculated." << std::endl;
        _outputStreamPtr->flush();
      }
      *p_jp = *local_jp;
    }
    else {
      if( _ignoreErrors ) {
        if( _verbose ) {
          *_outputStreamPtr << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored." 
                            << std::endl;
          *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                            << ret
                            << " is being ignored." 
                            << std::endl;
          _outputStreamPtr->flush();
          _errorStreamPtr->flush();
        }
        *p_jp = *local_jp;
        ret = 0;
      }
      else {
        if( _verbose ) {
          *_errorStreamPtr << "DispersionSage -- Closed orbit not successfully calculated." << std::endl;
          *_errorStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
          _errorStreamPtr->flush();
        }
        if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
        if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
        if( local_jp       ) { delete local_jp;        local_jp = 0;      }
        Jet__environment::setLastEnv( storedEnv );
        return ret;
      }
    }
  }


  // At this point,  *p_jp is on a closed orbit, with
  //   appropriate Jet__environment. 
  //   For purposes of paranoia, I reset Jet::_lastEnv here.
  Jet__environment::setLastEnv( p_jp->State().Env() );


  // Calculate the closed orbit for an off-momentum particle ...
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage --- Starting calculation of offset closed orbit." << std::endl;
    _outputStreamPtr->flush();
  }

  if( local_jp       ) { delete local_jp;      local_jp = 0;      }
  local_jp = p_jp->Clone();
  if( firstParticle  ) { delete firstParticle; firstParticle = 0; }
  firstParticle  = p_jp->ConvertToParticle();
  firstJacobian  = p_jp->State().Jacobian();
  energy = firstParticle->ReferenceEnergy();
  mass = firstParticle->Mass();
  momentum = sqrt( energy*energy - mass*mass )*( 1.0 + _dpp );
  energy = sqrt( momentum*momentum + mass*mass );

  local_jp->SetReferenceEnergy( energy );

  clsg.setForcedCalc();
  ret = clsg.findClosedOrbit( local_jp );
  clsg.unsetForcedCalc();

  // Review the solution
  if( ret == 0 ) {
    if( _verbose ) {
      *_outputStreamPtr << "DispersionSage -- Offset closed orbit successfully calculated." << std::endl;
      _outputStreamPtr->flush();
    }
  }
  else {
    if( _ignoreErrors ) {
      *_outputStreamPtr << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << std::endl;
      _outputStreamPtr->flush();
      _errorStreamPtr->flush();

      ret = 0;
    }
    else {
      if( _verbose ) {
        *_outputStreamPtr << "DispersionSage -- Off-momentum closed orbit not successfully calculated." << std::endl;
        *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
        _outputStreamPtr->flush();
      }

      if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
      if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
      if( local_jp       ) { delete local_jp;        local_jp = 0;      }
      Jet__environment::setLastEnv( storedEnv );
      return ret;
    }
  }

  if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
  secondParticle  = local_jp->ConvertToParticle();
  secondJacobian  = local_jp->State().Jacobian();


  // Attach initial dispersion data to the beamline ...
  Vector d( firstParticle->State().Dim() );


  // Attach dispersion data wherever desired ...
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage --- Attaching dispersion data to the elements." << std::endl;
    _outputStreamPtr->flush();
  }

  bmlnElmnt* q = 0;
  for( int counter = 0; counter < _n; counter++ ) 
  {
    q = _arrayPtr->e( counter );
    q->propagate( *firstParticle );
    q->propagate( *secondParticle );

    lng += q->OrbitLength( *firstParticle );

    d = ( secondParticle->State()  -  firstParticle->State() ) / _dpp;
  
    _calcs[counter].closedOrbit.hor  = firstParticle->get_x();
    _calcs[counter].closedOrbit.ver = firstParticle->get_y();
    _calcs[counter].dispersion.hor  = d( i_x  );
    _calcs[counter].dPrime.hor      = d( i_px );
    _calcs[counter].dispersion.ver  = d( i_y  );
    _calcs[counter].dPrime.ver      = d( i_py );
    _calcs[counter].arcLength       = lng;
  }  


  // Attach tune and chromaticity to the beamline ........
  Vector firstNu(2), secondNu(2);
  if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) && 
      ( 0 == filterTransverseTunes( secondJacobian, secondNu ) ) )
  {
    _lr.tune.hor = firstNu(0);
    _lr.tune.ver = firstNu(1);
    _lr.chromaticity.hor = ( secondNu(0) - firstNu(0) ) / _dpp;
    _lr.chromaticity.ver = ( secondNu(1) - firstNu(1) ) / _dpp;
  }
  else {
    *_errorStreamPtr 
         << "\n*** ERROR ***"
         << "\n*** ERROR *** File: " << __FILE__ << " Line: " << __LINE__
         << "\n*** ERROR *** DispersionSage::fullCalc"
            "\n*** ERROR *** Horrible, inexplicable error occurred"
            "\n*** ERROR *** while trying to filter the tunes."
            "\n*** ERROR ***"
         << std::endl;
    ret = 111;
    if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
    if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
    if( local_jp       ) { delete local_jp;        local_jp = 0;      }
    Jet__environment::setLastEnv( storedEnv );
    return ret;
  }


  // Final operations ....................................
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
  if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
  if( local_jp       ) { delete local_jp;        local_jp = 0;      }
  Jet__environment::setLastEnv( storedEnv );
  return ret;
}


int DispersionSage::pushCalc( const Particle& prt, const Info& initialConditions )
{
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::pushCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  int ret = 0;
  int i;
  Vector d( prt.State().Dim() );

  _deleteCalcs();
  _finishConstructor();

  Particle* firstParticle  = prt.Clone();
  Particle* secondParticle = prt.Clone();

  short int i_x   =  Particle::_x();
  short int i_y   =  Particle::_y();
  short int i_z   =  Particle::_cdt();
  short int i_px  =  Particle::_xp();
  short int i_py  =  Particle::_yp();
  short int i_dpp =  Particle::_dpop();

  double energy;
  double mass;
  double momentum;
  double lng = 0.0;

  energy = firstParticle->ReferenceEnergy();
  mass = firstParticle->Mass();
  momentum = sqrt( energy*energy - mass*mass )*( 1.0 + _dpp );
  energy = sqrt( momentum*momentum + mass*mass );
  secondParticle->SetReferenceEnergy( energy );

  secondParticle->set_x   (prt.get_x()   + (initialConditions.dispersion.hor)*_dpp);
  secondParticle->set_y   (prt.get_y()   + (initialConditions.dispersion.ver)*_dpp);
  secondParticle->set_npx (prt.get_npx() + (initialConditions.dPrime.hor)*_dpp);
  secondParticle->set_npy (prt.get_npy() + (initialConditions.dPrime.ver)*_dpp);

  DeepBeamlineIterator dbi( *_myBeamlinePtr );
  bmlnElmnt* q = 0;
  int counter = 0;
  while( (0 != (q = dbi++)) && (0 == ret) ) {
    if( counter < _n ) {
      q->propagate( *firstParticle );
      q->propagate( *secondParticle );
      lng += q->OrbitLength( *firstParticle );

      d = ( secondParticle->State() - firstParticle->State() ) / _dpp;
  
      _calcs[counter].dispersion.hor  = d( i_x  );
      _calcs[counter].dPrime.hor      = d( i_px );
      _calcs[counter].dispersion.ver  = d( i_y  );
      _calcs[counter].dPrime.ver      = d( i_py );
      _calcs[counter].arcLength       = lng;

      counter++;
    }
    else {
      ret = -1;
    }
  }

  // Clean up before leaving .............................................
  if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
  if( secondParticle ) { delete secondParticle;  secondParticle = 0;}

  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::pushCalc" << std::endl;
    _outputStreamPtr->flush();
  }

  return ret;
}


// POSTPONED int DispersionSage::fadCalc( const JetParticle* arg_jp, beamline::Criterion& crit ) const
// POSTPONED {
// POSTPONED   // 
// POSTPONED   // This is copied and modified from 
// POSTPONED   // int LattFuncSage::FAD_Disp_Calc( const JetParticle* arg_jp, 
// POSTPONED   //                                  Sage::CRITFUNC Crit )
// POSTPONED   // 
// POSTPONED   // NOTE: This routine does not calculate chromaticity.
// POSTPONED   // 
// POSTPONED 
// POSTPONED   // PRECONDITION: 
// POSTPONED 
// POSTPONED   if( _verbose ) {
// POSTPONED     *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::fadCalc" 
// POSTPONED          << std::endl;
// POSTPONED     _outputStreamPtr->flush();
// POSTPONED   }
// POSTPONED 
// POSTPONED   int ret = DispersionSage::DONE;
// POSTPONED   Particle* firstParticle = 0;
// POSTPONED 
// POSTPONED   short int i_x   =  Particle::_x();
// POSTPONED   short int i_y   =  Particle::_y();
// POSTPONED   short int i_z   =  Particle::_cdt();
// POSTPONED   short int i_px  =  Particle::_xp();
// POSTPONED   short int i_py  =  Particle::_yp();
// POSTPONED   short int i_dpp =  Particle::_dpop();
// POSTPONED 
// POSTPONED   short int j_x   =  0;
// POSTPONED   short int j_px  =  1;
// POSTPONED   short int j_y   =  2;
// POSTPONED   short int j_py  =  3;
// POSTPONED   short int j_dpp =  4;
// POSTPONED 
// POSTPONED   MatrixD FADmap( 5, 5 );
// POSTPONED   MatrixD firstJacobian;
// POSTPONED 
// POSTPONED 
// POSTPONED   // Calculate the 5x5 matrix ...
// POSTPONED   firstJacobian  = arg_jp->State().Jacobian();
// POSTPONED 
// POSTPONED   FADmap( j_x,   j_x   ) = firstJacobian( i_x,   i_x   );
// POSTPONED   FADmap( j_x,   j_px  ) = firstJacobian( i_x,   i_px  );
// POSTPONED   FADmap( j_x,   j_y   ) = firstJacobian( i_x,   i_y   );
// POSTPONED   FADmap( j_x,   j_py  ) = firstJacobian( i_x,   i_py  );
// POSTPONED   FADmap( j_x,   j_dpp ) = firstJacobian( i_x,   i_dpp );
// POSTPONED 
// POSTPONED   FADmap( j_px,  j_x   ) = firstJacobian( i_px,  i_x   );
// POSTPONED   FADmap( j_px,  j_px  ) = firstJacobian( i_px,  i_px  );
// POSTPONED   FADmap( j_px,  j_y   ) = firstJacobian( i_px,  i_y   );
// POSTPONED   FADmap( j_px,  j_py  ) = firstJacobian( i_px,  i_py  );
// POSTPONED   FADmap( j_px,  j_dpp ) = firstJacobian( i_px,  i_dpp );
// POSTPONED 
// POSTPONED   FADmap( j_y,   j_x   ) = firstJacobian( i_y,   i_x   );
// POSTPONED   FADmap( j_y,   j_px  ) = firstJacobian( i_y,   i_px  );
// POSTPONED   FADmap( j_y,   j_y   ) = firstJacobian( i_y,   i_y   );
// POSTPONED   FADmap( j_y,   j_py  ) = firstJacobian( i_y,   i_py  );
// POSTPONED   FADmap( j_y,   j_dpp ) = firstJacobian( i_y,   i_dpp );
// POSTPONED 
// POSTPONED   FADmap( j_py,  j_x   ) = firstJacobian( i_py,  i_x   );
// POSTPONED   FADmap( j_py,  j_px  ) = firstJacobian( i_py,  i_px  );
// POSTPONED   FADmap( j_py,  j_y   ) = firstJacobian( i_py,  i_y   );
// POSTPONED   FADmap( j_py,  j_py  ) = firstJacobian( i_py,  i_py  );
// POSTPONED   FADmap( j_py,  j_dpp ) = firstJacobian( i_py,  i_dpp );
// POSTPONED 
// POSTPONED   FADmap( j_dpp, j_x   ) = 0.0;
// POSTPONED   FADmap( j_dpp, j_px  ) = 0.0;
// POSTPONED   FADmap( j_dpp, j_y   ) = 0.0;
// POSTPONED   FADmap( j_dpp, j_py  ) = 0.0;
// POSTPONED   FADmap( j_dpp, j_dpp ) = 1.0;
// POSTPONED 
// POSTPONED 
// POSTPONED   // Finds its eigenvectors....
// POSTPONED   int i, j;
// POSTPONED   MatrixC EV;
// POSTPONED   MatrixC lambda;
// POSTPONED   EV = FADmap.eigenVectors();
// POSTPONED   lambda = FADmap.eigenValues();   // Wildly inefficient. Does the same
// POSTPONED                                    // calculation twice. But this is only
// POSTPONED                                    // a 5x5, so it shouldn't matter much.
// POSTPONED   
// POSTPONED   int numberFound = 0;
// POSTPONED   static const FNAL::Complex c_one( 1.0, 0.0 );
// POSTPONED   int theColumn;
// POSTPONED 
// POSTPONED   for( i = 0; i < 5; i++ ) {
// POSTPONED     if( abs( EV(j_dpp,i) ) > 1.0e-6 ) {
// POSTPONED       numberFound++;
// POSTPONED       theColumn = i;
// POSTPONED       if( abs( EV(j_dpp,i) - c_one ) > 1.0e-6 ) {
// POSTPONED         for( j = 0; j < 5; j++ ) {
// POSTPONED           EV(j,i) = EV(j,i) / EV(j_dpp,i);
// POSTPONED         }
// POSTPONED       }
// POSTPONED     }
// POSTPONED   }
// POSTPONED 
// POSTPONED   if( numberFound != 1 ) {
// POSTPONED     *_errorStreamPtr << "*** ERROR ***                                            \n"
// POSTPONED             "*** ERROR *** DispersionSage::FAD_Disp_Calc                \n"
// POSTPONED             "*** ERROR *** Exactly " << numberFound << " eigenvectors \n"
// POSTPONED             "*** ERROR *** were found matching the criterion.         \n"
// POSTPONED             "*** ERROR *** Results of this calculation will not be    \n"
// POSTPONED             "*** ERROR *** correct.                                   \n"
// POSTPONED             "*** ERROR ***                                            \n"
// POSTPONED          << std::endl;
// POSTPONED     ret = DispersionSage::TOO_MANY_VECTORS;
// POSTPONED   }
// POSTPONED 
// POSTPONED   else { // Continue
// POSTPONED      double lng = 0.0;
// POSTPONED 
// POSTPONED      // Attach initial dispersion data to the beamline ...
// POSTPONED      DispersionSage::Info* lf = new DispersionSage::Info;
// POSTPONED      lf->dispersion.hor = real( EV( j_x,  theColumn ) );
// POSTPONED      lf->dPrime.hor     = real( EV( j_px, theColumn ) );
// POSTPONED      lf->dispersion.ver = real( EV( j_y,  theColumn ) );
// POSTPONED      lf->dPrime.ver     = real( EV( j_py, theColumn ) );
// POSTPONED      lf->arcLength      = lng;
// POSTPONED      
// POSTPONED      _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );
// POSTPONED      _myBeamlinePtr->dataHook.insert( new Barnacle( "DispersionSage", lf ) );
// POSTPONED    
// POSTPONED      // Attach dispersion data wherever desired ...
// POSTPONED      if( _verbose ) {
// POSTPONED        *_outputStreamPtr << "DispersionSage --- Attaching dispersion data to the elements." << std::endl;
// POSTPONED        _outputStreamPtr->flush();
// POSTPONED      }
// POSTPONED    
// POSTPONED    
// POSTPONED      bmlnElmnt* q = 0;
// POSTPONED      firstParticle  = arg_jp->ConvertToParticle();
// POSTPONED      DeepBeamlineIterator getNext( _myBeamlinePtr );
// POSTPONED      double dpp = this->get_dpp();
// POSTPONED 
// POSTPONED      firstParticle->set_x  ( dpp*real( EV( j_x,  theColumn ) ) );
// POSTPONED      firstParticle->set_y  ( dpp*real( EV( j_y,  theColumn ) ) );
// POSTPONED      firstParticle->set_cdt( 0.0 );
// POSTPONED      firstParticle->set_npx( dpp*real( EV( j_px, theColumn ) ) );
// POSTPONED      firstParticle->set_npy( dpp*real( EV( j_py, theColumn ) ) );
// POSTPONED      firstParticle->set_ndp( 0.0 );
// POSTPONED 
// POSTPONED      firstParticle->SetReferenceMomentum( ( 1.0 + dpp )*(firstParticle->ReferenceMomentum()) );
// POSTPONED 
// POSTPONED      while((  q = getNext++  )) 
// POSTPONED      {
// POSTPONED        q->propagate( *firstParticle );
// POSTPONED        lng += q->OrbitLength( *firstParticle );
// POSTPONED    
// POSTPONED        if( crit( q ) ) 
// POSTPONED        {
// POSTPONED          lf = new DispersionSage::Info;
// POSTPONED          lf->dispersion.hor = firstParticle->get_x()   /dpp;
// POSTPONED          lf->dPrime.hor     = firstParticle->get_npx() /dpp;
// POSTPONED          lf->dispersion.ver = firstParticle->get_y()   /dpp;
// POSTPONED          lf->dPrime.ver     = firstParticle->get_npy() /dpp;
// POSTPONED          lf->arcLength      = lng;
// POSTPONED      
// POSTPONED          q->dataHook.eraseAll( "DispersionSage" );
// POSTPONED          q->dataHook.insert( new Barnacle( "DispersionSage", lf ) );
// POSTPONED        }    
// POSTPONED      }  
// POSTPONED    
// POSTPONED 
// POSTPONED      // Attach tune and chromaticity to the beamline ........
// POSTPONED      DispersionSage::GlobalInfo* latticeRing = new DispersionSage::GlobalInfo;
// POSTPONED      Vector firstNu(2);
// POSTPONED      if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) )
// POSTPONED      {
// POSTPONED        latticeRing->tune.hor = firstNu(0);
// POSTPONED        latticeRing->tune.ver = firstNu(1);
// POSTPONED        latticeRing->chromaticity.hor = 12345.6789;  // Nonsense value.
// POSTPONED        latticeRing->chromaticity.ver = 12345.6789;
// POSTPONED        _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );
// POSTPONED        _myBeamlinePtr->dataHook.insert( new Barnacle( "DispersionSage", latticeRing ) );
// POSTPONED      }
// POSTPONED      else {
// POSTPONED        *_errorStreamPtr << "*** ERROR ***                                        \n"
// POSTPONED                "*** ERROR *** DispersionSage::fadCalc\n"
// POSTPONED                "*** ERROR ***                                        \n"
// POSTPONED                "*** ERROR *** Horrible error occurred while trying   \n"
// POSTPONED                "*** ERROR *** to filter the tunes.                   \n"
// POSTPONED                "*** ERROR ***                                        \n"
// POSTPONED             << std::endl;
// POSTPONED        ret = DispersionSage::IMPOSSIBILITY;
// POSTPONED      }  
// POSTPONED 
// POSTPONED   } // else continue
// POSTPONED 
// POSTPONED   // Final operations ....................................
// POSTPONED   if( _verbose ) {
// POSTPONED     *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::Disp_Calc" << std::endl;
// POSTPONED     _outputStreamPtr->flush();
// POSTPONED   }
// POSTPONED 
// POSTPONED   if( firstParticle ) { delete firstParticle; firstParticle = 0; }
// POSTPONED   return ret;
// POSTPONED }


const DispersionSage::Info* DispersionSage::getInfoPtr() const
{
  return &(_calcs[_n-1]);
}


const DispersionSage::Info* DispersionSage::getInfoPtr( int i ) const
{
  if( 0 <= i && i < _n ) { 
    return &(_calcs[i]); 
  }
  return 0;
}


// REMOVE: bool DispersionSage::checkInfoPtr() const
// REMOVE: {
// REMOVE:   bmlnElmnt* be = _myBeamlinePtr->lastElement();
// REMOVE:   if( _calcs[_n-1] == 
// REMOVE:       dynamic_cast<DispersionSage::Info*>
// REMOVE:       (be->dataHook.find( "DispersionSage" )) 
// REMOVE:     ) 
// REMOVE:   { return true; }
// REMOVE:   
// REMOVE:   return false;
// REMOVE: }


// REMOVE: bool DispersionSage::checkInfoPtr( int i ) const
// REMOVE: {
// REMOVE:   int k = 0; 
// REMOVE:   DeepBeamlineIterator dbi( _myBeamlinePtr );
// REMOVE:   bmlnElmnt* be = 0;
// REMOVE:   while((  (k < _n) && (be = dbi++)  )) {
// REMOVE:     if( k == i ) {
// REMOVE:       if( _calcs[i] == 
// REMOVE:           dynamic_cast<DispersionSage::Info*>
// REMOVE:           (be->dataHook.find( "DispersionSage" )) 
// REMOVE:         ) 
// REMOVE:       { return true; }
// REMOVE:     }
// REMOVE:     k++;
// REMOVE:   }
// REMOVE:   
// REMOVE:   return false;
// REMOVE: }


void DispersionSage::eraseAll()
{
  _deleteCalcs();
  _finishConstructor();

  if( _n != _myBeamlinePtr->countHowManyDeeply() ) {
    *_errorStreamPtr << "\n*** WARNING ***"
                        "\n*** WARNING *** File: " << __FILE__ 
                     <<                  " Line: " << __LINE__
                     << "\n*** WARNING *** DispersionSage::eraseAll"
                        "\n*** WARNING *** Mismatch in number of elements."
                        "\n*** WARNING *** Has the beamline been edited?"
                        "\n*** WARNING ***"
                     << std::endl;
  }
}


void DispersionSage::set_dpp( double x )
{
  if( x > 0.0 ) { _dpp = x; }
  else {
    *_errorStreamPtr << "\n*** WARNING ***                         "
                        "\n*** WARNING *** DispersionSage::set_dpp( "
                     << x 
                     << " )"
                        "\n*** WARNING *** Non-positive argument changed."
                        "\n*** WARNING ***                         "
                     << std::endl;
    if( x != 0.0 ) { _dpp = -x; }
  }
}
