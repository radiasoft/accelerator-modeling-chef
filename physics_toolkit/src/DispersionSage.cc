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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "ClosedOrbitSage.h"
#include "DispersionSage.h"

extern int filterTransverseTunes( /* const */ MatrixD&, Vector& );

// ... Globals:
const int DispersionSage::DONE             = 0;
const int DispersionSage::TOO_MANY_VECTORS = 7;
const int DispersionSage::IMPOSSIBILITY    = 111;


DispersionSage::Info::Info()
{
  arcLength = -1.0;
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
  _calcs(0),
  _n(0)
{
  // REMOVE: _finishConstructor();
}

DispersionSage::DispersionSage( const beamline& x, bool doClone )
: Sage( &x, doClone ), 
  _dpp( 0.00005 ), 
  _ignoreErrors( false ),
  _calcs(0),
  _n(0)
{
  // REMOVE: _finishConstructor();
}

void DispersionSage::_finishConstructor()
{
  _n = _arrayPtr->size();
  if( 0 < _n ) {
    _calcs = new Info* [_n];
    _calcs[0] = new Info[_n];
    Info* k = _calcs[0];
    for( int i = 1; i < _n; i++ ) { _calcs[i] = k++; }
  }
}

DispersionSage::~DispersionSage()
{
  _deleteCalcs();
}

void DispersionSage::_deleteCalcs()
{
  _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );

  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be = 0;
  while((  be = dbi++  )) {
    be->dataHook.eraseAll( "DispersionSage" );
    be->dataHook.eraseAll( "DispersionSage" );
  }

  if( _calcs ) { 
    for( int i = 0; i < _n; i++ )
    { if( 0 != _calcs[i] ) {delete _calcs[i];} }
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
    *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::fullCalc" << endl;
    _outputStreamPtr->flush();
  }

  // Create the _calcs array
  _deleteCalcs();
  _finishConstructor();

  // Preserve the current Jet environment
  Jet__environment* storedEnv = Jet::_lastEnv;

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
  if( _verbose ) {
    clsg.set_verbose();
  }


  // If the argument is not already declared
  //   to be on the closed orbit, then use a
  //   ClosedOrbitSage to rebuild it.
  JetParticle* local_jp = p_jp->Clone();
  if( !(this->flags.onClosedOrbit) ) { 
    clsg.setForcedCalc(); 
    ret = clsg.findClosedOrbit( local_jp );
    clsg.unsetForcedCalc();
  }


  // Review the solution
  if( ret == 0 ) {
    if( _verbose ) {
      *_outputStreamPtr << "DispersionSage -- Closed orbit successfully calculated." << endl;
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
                          << endl;
        *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                          << ret
                          << " is being ignored." 
                          << endl;
        _outputStreamPtr->flush();
        _errorStreamPtr->flush();
      }

      *p_jp = *local_jp;
      ret = 0;
    }
    else {
      if( _verbose ) {
        *_errorStreamPtr << "DispersionSage -- Closed orbit not successfully calculated." << endl;
        *_errorStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << endl;
        _errorStreamPtr->flush();
      }

      if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
      if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
      if( local_jp       ) { delete local_jp;        local_jp = 0;      }
      Jet::_lastEnv = storedEnv;
      return ret;
    }
  }


  // At this point,  *p_jp is on a closed orbit, with
  //   appropriate Jet__environment. 
  //   For purposes of paranoia, I reset Jet::_lastEnv here.
  Jet::_lastEnv = (Jet__environment*) p_jp->State().Env();


  // Calculate the closed orbit for an off-momentum particle ...
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage --- Starting calculation of offset closed orbit." << endl;
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
      *_outputStreamPtr << "DispersionSage -- Offset closed orbit successfully calculated." << endl;
      _outputStreamPtr->flush();
    }
  }
  else {
    if( _ignoreErrors ) {
      *_outputStreamPtr << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << endl;
      *_errorStreamPtr  << "DispersionSage -- ClosedOrbitSage error "
                        << ret
                        << " is being ignored for offset orbit." 
                        << endl;
      _outputStreamPtr->flush();
      _errorStreamPtr->flush();

      ret = 0;
    }
    else {
      if( _verbose ) {
        *_outputStreamPtr << "DispersionSage -- Off-momentum closed orbit not successfully calculated." << endl;
        *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << endl;
        _outputStreamPtr->flush();
      }

      if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
      if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
      if( local_jp       ) { delete local_jp;        local_jp = 0;      }
      Jet::_lastEnv = storedEnv;
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
    *_outputStreamPtr << "DispersionSage --- Attaching dispersion data to the elements." << endl;
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
  
    _calcs[counter]->dispersion.hor = d( i_x  );
    _calcs[counter]->dPrime.hor     = d( i_px );
    _calcs[counter]->dispersion.ver = d( i_y  );
    _calcs[counter]->dPrime.ver     = d( i_py );
    _calcs[counter]->arcLength      = lng;
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
         << endl;
    ret = 111;
    if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
    if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
    if( local_jp       ) { delete local_jp;        local_jp = 0;      }
    Jet::_lastEnv = storedEnv;
    return ret;
  }


  // Final operations ....................................
  if( _verbose ) {
    *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::fullCalc" << endl;
    _outputStreamPtr->flush();
  }

  if( firstParticle  ) { delete firstParticle;   firstParticle = 0; }
  if( secondParticle ) { delete secondParticle;  secondParticle = 0;}
  if( local_jp       ) { delete local_jp;        local_jp = 0;      }
  Jet::_lastEnv = storedEnv;
  return ret;
}


// int DispersionSage::fadCalc( const JetParticle* arg_jp, beamline::Criterion& crit ) const
// {
//   // 
//   // This is copied and modified from 
//   // int LattFuncSage::FAD_Disp_Calc( const JetParticle* arg_jp, 
//   //                                  Sage::CRITFUNC Crit )
//   // 
//   // NOTE: This routine does not calculate chromaticity.
//   // 
// 
//   // PRECONDITION: 
// 
//   if( _verbose ) {
//     *_outputStreamPtr << "DispersionSage -- Entering DispersionSage::fadCalc" 
//          << endl;
//     _outputStreamPtr->flush();
//   }
// 
//   int ret = DispersionSage::DONE;
//   Particle* firstParticle = 0;
// 
//   short int i_x   =  Particle::_x();
//   short int i_y   =  Particle::_y();
//   short int i_z   =  Particle::_cdt();
//   short int i_px  =  Particle::_xp();
//   short int i_py  =  Particle::_yp();
//   short int i_dpp =  Particle::_dpop();
// 
//   short int j_x   =  0;
//   short int j_px  =  1;
//   short int j_y   =  2;
//   short int j_py  =  3;
//   short int j_dpp =  4;
// 
//   MatrixD FADmap( 5, 5 );
//   MatrixD firstJacobian;
// 
// 
//   // Calculate the 5x5 matrix ...
//   firstJacobian  = arg_jp->State().Jacobian();
// 
//   FADmap( j_x,   j_x   ) = firstJacobian( i_x,   i_x   );
//   FADmap( j_x,   j_px  ) = firstJacobian( i_x,   i_px  );
//   FADmap( j_x,   j_y   ) = firstJacobian( i_x,   i_y   );
//   FADmap( j_x,   j_py  ) = firstJacobian( i_x,   i_py  );
//   FADmap( j_x,   j_dpp ) = firstJacobian( i_x,   i_dpp );
// 
//   FADmap( j_px,  j_x   ) = firstJacobian( i_px,  i_x   );
//   FADmap( j_px,  j_px  ) = firstJacobian( i_px,  i_px  );
//   FADmap( j_px,  j_y   ) = firstJacobian( i_px,  i_y   );
//   FADmap( j_px,  j_py  ) = firstJacobian( i_px,  i_py  );
//   FADmap( j_px,  j_dpp ) = firstJacobian( i_px,  i_dpp );
// 
//   FADmap( j_y,   j_x   ) = firstJacobian( i_y,   i_x   );
//   FADmap( j_y,   j_px  ) = firstJacobian( i_y,   i_px  );
//   FADmap( j_y,   j_y   ) = firstJacobian( i_y,   i_y   );
//   FADmap( j_y,   j_py  ) = firstJacobian( i_y,   i_py  );
//   FADmap( j_y,   j_dpp ) = firstJacobian( i_y,   i_dpp );
// 
//   FADmap( j_py,  j_x   ) = firstJacobian( i_py,  i_x   );
//   FADmap( j_py,  j_px  ) = firstJacobian( i_py,  i_px  );
//   FADmap( j_py,  j_y   ) = firstJacobian( i_py,  i_y   );
//   FADmap( j_py,  j_py  ) = firstJacobian( i_py,  i_py  );
//   FADmap( j_py,  j_dpp ) = firstJacobian( i_py,  i_dpp );
// 
//   FADmap( j_dpp, j_x   ) = 0.0;
//   FADmap( j_dpp, j_px  ) = 0.0;
//   FADmap( j_dpp, j_y   ) = 0.0;
//   FADmap( j_dpp, j_py  ) = 0.0;
//   FADmap( j_dpp, j_dpp ) = 1.0;
// 
// 
//   // Finds its eigenvectors....
//   int i, j;
//   MatrixC EV;
//   MatrixC lambda;
//   EV = FADmap.eigenVectors();
//   lambda = FADmap.eigenValues();   // Wildly inefficient. Does the same
//                                    // calculation twice. But this is only
//                                    // a 5x5, so it shouldn't matter much.
//   
//   int numberFound = 0;
//   static const FNAL::Complex c_one( 1.0, 0.0 );
//   int theColumn;
// 
//   for( i = 0; i < 5; i++ ) {
//     if( abs( EV(j_dpp,i) ) > 1.0e-6 ) {
//       numberFound++;
//       theColumn = i;
//       if( abs( EV(j_dpp,i) - c_one ) > 1.0e-6 ) {
//         for( j = 0; j < 5; j++ ) {
//           EV(j,i) = EV(j,i) / EV(j_dpp,i);
//         }
//       }
//     }
//   }
// 
//   if( numberFound != 1 ) {
//     *_errorStreamPtr << "*** ERROR ***                                            \n"
//             "*** ERROR *** DispersionSage::FAD_Disp_Calc                \n"
//             "*** ERROR *** Exactly " << numberFound << " eigenvectors \n"
//             "*** ERROR *** were found matching the criterion.         \n"
//             "*** ERROR *** Results of this calculation will not be    \n"
//             "*** ERROR *** correct.                                   \n"
//             "*** ERROR ***                                            \n"
//          << endl;
//     ret = DispersionSage::TOO_MANY_VECTORS;
//   }
// 
//   else { // Continue
//      double lng = 0.0;
// 
//      // Attach initial dispersion data to the beamline ...
//      DispersionSage::Info* lf = new DispersionSage::Info;
//      lf->dispersion.hor = real( EV( j_x,  theColumn ) );
//      lf->dPrime.hor     = real( EV( j_px, theColumn ) );
//      lf->dispersion.ver = real( EV( j_y,  theColumn ) );
//      lf->dPrime.ver     = real( EV( j_py, theColumn ) );
//      lf->arcLength      = lng;
//      
//      _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );
//      _myBeamlinePtr->dataHook.insert( new Barnacle( "DispersionSage", lf ) );
//    
//      // Attach dispersion data wherever desired ...
//      if( _verbose ) {
//        *_outputStreamPtr << "DispersionSage --- Attaching dispersion data to the elements." << endl;
//        _outputStreamPtr->flush();
//      }
//    
//    
//      bmlnElmnt* q = 0;
//      firstParticle  = arg_jp->ConvertToParticle();
//      DeepBeamlineIterator getNext( _myBeamlinePtr );
//      double dpp = this->get_dpp();
// 
//      firstParticle->set_x  ( dpp*real( EV( j_x,  theColumn ) ) );
//      firstParticle->set_y  ( dpp*real( EV( j_y,  theColumn ) ) );
//      firstParticle->set_cdt( 0.0 );
//      firstParticle->set_npx( dpp*real( EV( j_px, theColumn ) ) );
//      firstParticle->set_npy( dpp*real( EV( j_py, theColumn ) ) );
//      firstParticle->set_ndp( 0.0 );
// 
//      firstParticle->SetReferenceMomentum( ( 1.0 + dpp )*(firstParticle->ReferenceMomentum()) );
// 
//      while((  q = getNext++  )) 
//      {
//        q->propagate( *firstParticle );
//        lng += q->OrbitLength( *firstParticle );
//    
//        if( crit( q ) ) 
//        {
//          lf = new DispersionSage::Info;
//          lf->dispersion.hor = firstParticle->get_x()   /dpp;
//          lf->dPrime.hor     = firstParticle->get_npx() /dpp;
//          lf->dispersion.ver = firstParticle->get_y()   /dpp;
//          lf->dPrime.ver     = firstParticle->get_npy() /dpp;
//          lf->arcLength      = lng;
//      
//          q->dataHook.eraseAll( "DispersionSage" );
//          q->dataHook.insert( new Barnacle( "DispersionSage", lf ) );
//        }    
//      }  
//    
// 
//      // Attach tune and chromaticity to the beamline ........
//      DispersionSage::GlobalInfo* latticeRing = new DispersionSage::GlobalInfo;
//      Vector firstNu(2);
//      if( ( 0 == filterTransverseTunes( firstJacobian, firstNu   ) ) )
//      {
//        latticeRing->tune.hor = firstNu(0);
//        latticeRing->tune.ver = firstNu(1);
//        latticeRing->chromaticity.hor = 12345.6789;  // Nonsense value.
//        latticeRing->chromaticity.ver = 12345.6789;
//        _myBeamlinePtr->dataHook.eraseAll( "DispersionSage" );
//        _myBeamlinePtr->dataHook.insert( new Barnacle( "DispersionSage", latticeRing ) );
//      }
//      else {
//        *_errorStreamPtr << "*** ERROR ***                                        \n"
//                "*** ERROR *** DispersionSage::fadCalc\n"
//                "*** ERROR ***                                        \n"
//                "*** ERROR *** Horrible error occurred while trying   \n"
//                "*** ERROR *** to filter the tunes.                   \n"
//                "*** ERROR ***                                        \n"
//             << endl;
//        ret = DispersionSage::IMPOSSIBILITY;
//      }  
// 
//   } // else continue
// 
//   // Final operations ....................................
//   if( _verbose ) {
//     *_outputStreamPtr << "DispersionSage -- Leaving DispersionSage::Disp_Calc" << endl;
//     _outputStreamPtr->flush();
//   }
// 
//   if( firstParticle ) { delete firstParticle; firstParticle = 0; }
//   return ret;
// }


const DispersionSage::Info* DispersionSage::getInfoPtr() const
{
  return _calcs[_n-1];
}


const DispersionSage::Info* DispersionSage::getInfoPtr( int i ) const
{
  if( 0 <= i && i < _n ) { return _calcs[i]; }
  return 0;
}


bool DispersionSage::checkInfoPtr() const
{
  bmlnElmnt* be = _myBeamlinePtr->lastElement();
  if( _calcs[_n-1] == 
      dynamic_cast<DispersionSage::Info*>
      (be->dataHook.find( "DispersionSage" )) 
    ) 
  { return true; }
  
  return false;
}


bool DispersionSage::checkInfoPtr( int i ) const
{
  int k = 0; 
  DeepBeamlineIterator dbi( _myBeamlinePtr );
  bmlnElmnt* be = 0;
  while((  (k < _n) && (be = dbi++)  )) {
    if( k == i ) {
      if( _calcs[i] == 
          dynamic_cast<DispersionSage::Info*>
          (be->dataHook.find( "DispersionSage" )) 
        ) 
      { return true; }
    }
    k++;
  }
  
  return false;
}


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
                     << endl;
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
                     << endl;
    if( x != 0.0 ) { _dpp = -x; }
  }
}
