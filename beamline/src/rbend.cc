/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.2
******                                    
******  File:      rbend.cc
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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

#include <iomanip>
#include <typeinfo>

#include "MathConstants.h"
#include "PhysicsConstants.h"
#include "rbend.h"
#include "Particle.h"

using namespace std;

// **************************************************
//   class rbend
// **************************************************

rbend::rbend( double l, double s, PropFunc* pf ) 
: bmlnElmnt( l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
  , _myArcsin(true)
{
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 _calcPropParams();
}


rbend::rbend( const char* n, double l, double s, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
  , _myArcsin(true)
{
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 _calcPropParams();
}


rbend::rbend( double l, double s, double entryangle, PropFunc* pf ) 
: bmlnElmnt( l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(entryangle)
  , _dsAngle(-entryangle)
  , _usTan(tan(entryangle))
  , _dsTan(-tan(entryangle))
  , _myArcsin(true)
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-6) ) {
   _usAngle = 0.0;
   _usTan   = 0.0;
   _dsAngle = 0.0;
   _dsTan   = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 _calcPropParams();
}


rbend::rbend( const char* n, double l, double s, double entryangle, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(entryangle)
  , _dsAngle(-entryangle)
  , _usTan(tan(entryangle))
  , _dsTan(-tan(entryangle))
  , _myArcsin(true)
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-6) ) {
   _usAngle = 0.0;
   _usTan   = 0.0;
   _dsAngle = 0.0;
   _dsTan   = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 _calcPropParams();
}


rbend::rbend( double l, double s, double us, double ds, PropFunc* pf )
:   bmlnElmnt( l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
  , _myArcsin(true)
{
 static bool firstTime = true;
 if ( fabs( us ) < 1.0e-6 ) {
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( ds ) < 1.0e-6 ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = 0.0;
   _dsTan = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double l, double s, double us, double ds, PropFunc* pf )
:   bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
  , _myArcsin(true)
{
 static bool firstTime = true;
 if ( fabs( us ) < 1.0e-6 ) {
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( ds ) < 1.0e-6 ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = 0.0;
   _dsTan = 0.0;
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( double l, double s, double entryangle, double us, double ds, PropFunc* pf )
:   bmlnElmnt( l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(entryangle + us)
  , _dsAngle(-(entryangle + ds))
  , _usTan(tan(entryangle + us))
  , _dsTan(-tan(entryangle + ds))
  , _myArcsin(true)
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-6) ) {
   _usAngle = us;
   _usTan   = tan(us);
   _dsAngle = -ds;
   _dsTan   = -tan(ds);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( us ) < 1.0e-6 ) {
   _usEdgeAngle = 0.0;
   _usAngle = entryangle;
   _usTan = tan(entryangle);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( ds ) < 1.0e-6 ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = -entryangle;
   _dsTan = -tan(entryangle);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double l, double s, double entryangle, double us, double ds, PropFunc* pf )
:   bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(entryangle + us)
  , _dsAngle(-(entryangle + ds))
  , _usTan(tan(entryangle + us))
  , _dsTan(-tan(entryangle + ds))
  , _myArcsin(true)
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-6) ) {
   _usAngle = us;
   _usTan   = tan(us);
   _dsAngle = -ds;
   _dsTan   = -tan(ds);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( us ) < 1.0e-6 ) {
   _usEdgeAngle = 0.0;
   _usAngle = entryangle;
   _usTan = tan(entryangle);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( fabs( ds ) < 1.0e-6 ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = -entryangle;
   _dsTan = -tan(entryangle);
   if( firstTime) {
     cerr <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }

 _calcPropParams();
}


void rbend::_calcPropParams()
{
  // For edge focussing kludge
  _usTan = tan( _usAngle );
  _dsTan = tan( _dsAngle );

  // Geometric parameters
  double psi = - (_usEdgeAngle + _dsEdgeAngle);
  _propPhase = FNAL::Complex( cos(psi), sin(psi) );

  _propTerm =   this->Length()
              * FNAL::Complex( cos(_dsEdgeAngle), -sin(_dsEdgeAngle) );
}


rbend::rbend( const rbend& x )
: bmlnElmnt( (bmlnElmnt&) x )
  , _usEdgeAngle(x._usEdgeAngle)
  , _dsEdgeAngle(x._dsEdgeAngle)
  , _usAngle(x._usAngle)
  , _dsAngle(x._dsAngle)
  , _usTan(x._usTan)
  , _dsTan(x._dsTan)
  , _propPhase(x._propPhase)
  , _propTerm(x._propTerm)
  , _myArcsin(x._myArcsin)
{
}


rbend::~rbend() 
{
}


void rbend::eliminate() {
 delete this;
}


const char* rbend::Type() const { 
  return "rbend"; 
}


double rbend::OrbitLength( const Particle& x )
{
  // Computes arclength of orbit assuming a symmetric bend.
  // WARNING: This is not the true arclength under all conditions.
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    cerr << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  if( typeid(*Propagator) == typeid(MAD_Prop) ) {
    cerr << "\n*** WARNING *** "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** Splitting rbend with MAD-like propagator."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** You'll get wrong results, "
                              "but it's your fault for using this propagator."
            "\n*** WARNING *** "
         << endl;
    *a = new rbend( pc*length, strength, 
                    pc*_usAngle, // this is surely the wrong thing to do
                    Propagator );     // but there is no right thing
    *b = new rbend( (1.0 - pc)*length, strength, 
                    (1.0 - pc)*_usAngle,
                    Propagator );
  }
  else if( typeid(*Propagator) == typeid(NoEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, Propagator );
  }
  else if( typeid(*Propagator) == typeid(Exact_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, &rbend::InEdge );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, &rbend::OutEdge );
  }
  else if( typeid(*Propagator) == typeid(InEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, &rbend::NoEdge );
  }
  else if( typeid(*Propagator) == typeid(OutEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, &rbend::NoEdge );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, Propagator );
  }
  else if( typeid(*Propagator) == typeid(Real_Exact_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, &rbend::RealInEdge );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, &rbend::RealOutEdge );
  }
  else if( typeid(*Propagator) == typeid(Real_InEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, &rbend::NoEdge );
  }
  else if( typeid(*Propagator) == typeid(Real_OutEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, &rbend::NoEdge );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, Propagator );
  }
  else {
    cerr << "\n*** WARNING *** "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** Propagator type unrecognized."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** It's all your fault."
            "\n*** WARNING *** "
         << endl;
    *a = new rbend(         pc*length, strength, _usEdgeAngle, 0.0, &rbend::NoEdge );
    *b = new rbend( (1.0 - pc)*length, strength, 0.0, _dsEdgeAngle, &rbend::NoEdge );
  }

  // Rename
  char* newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_1" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_2" );
  (*b)->Rename( newname );

  delete [] newname;
}


double rbend::setPoleFaceAngle( const Particle& p )
{
  double psi =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );
  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)
  //                  = 1/2 symmetric bend angle
  this->setEntryAngle( psi );
  this->setExitAngle( -psi );
  return _usAngle;
}


double rbend::setPoleFaceAngle( const JetParticle& p )
{
  double psi =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );
  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)
  //                  = 1/2 symmetric bend angle
  this->setEntryAngle( psi );
  this->setExitAngle( -psi );
  return _usAngle;
}


double rbend::setEntryAngle( const Particle& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double rbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double rbend::setEntryAngle( double phi /* radians */ )
{
  double ret = _usAngle;
  _usAngle = phi;
  _calcPropParams();
  return ret;
}


double rbend::setExitAngle( double phi /* radians */ )
{
  double ret = _dsAngle;
  _dsAngle = phi;  
  _calcPropParams();
  return ret;
}


void rbend::makeAsinApproximate( int n )
{
  _myArcsin.makeApproximate();
  _myArcsin.setNumTerms(n);
}


void rbend::makeAsinExact()
{
  _myArcsin.makeExact();
}


bool rbend::isAsinExact()
{
  return _myArcsin.isExact();
}


// REMOVE: double rbend::setReferenceTime( const Particle& prtn )
// REMOVE: {
// REMOVE:   // Assumes "normal" symmetric crossing.
// REMOVE:   static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
// REMOVE:   double Omega  = csq_red * this->Strength() / prtn.ReferenceEnergy();
// REMOVE:   double Rho  = prtn.ReferenceBRho() / this->Strength();
// REMOVE:   _CT = PH_MKS_c * ( 2.0*asin( this->Length() / (2.0*Rho) ) ) / Omega;
// REMOVE:   return _CT;
// REMOVE: }


// REMOVE: double rbend::setReferenceTime( double x )
// REMOVE: {
// REMOVE:   double oldValue = _CT;
// REMOVE:   _CT = x;
// REMOVE:   if( fabs(_CT) < 1.0e-12 ) { _CT = 0.0; }
// REMOVE:   return oldValue;
// REMOVE: }


ostream& rbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << _usEdgeAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsEdgeAngle << endl;
  os << OSTREAM_DOUBLE_PREC << _usAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsAngle << endl;

  // Determine which propogators are being used, and make a note of it.
  if ( Propagator ==            &rbend::Exact )
    os << "rbend::P_Exact    rbend::J_Exact";
  else if ( Propagator ==       &rbend::LikeMAD )
    os << "rbend::P_LikeMAD  rbend::J_LikeMAD";
  else if ( Propagator ==       &rbend::NoEdge )
    os << "rbend::P_NoEdge   rbend::J_NoEdge";
  else if ( Propagator ==       &rbend::InEdge )
    os << "rbend::P_InEdge   rbend::J_InEdge";
  else if ( Propagator ==       &rbend::OutEdge )
    os << "rbend::P_OutEdge  rbend::J_OutEdge";
  else
    os << "UNKNOWN  UNKNOWN";
  
  os << "\n";
  return os;
}


istream& rbend::readFrom(istream& is)
{
  is >> _usEdgeAngle >> _dsEdgeAngle;
  is >> _usAngle >> _dsAngle;

  char prop_fun[100], jet_prop_fun[100];
  is >> prop_fun >> jet_prop_fun;

  if ( strcasecmp(prop_fun,             "rbend::P_Exact" ) == 0 )
    setPropFunction(&rbend::Exact );
  else if ( strcasecmp(prop_fun,        "rbend::P_LikeMAD" ) == 0 )
    setPropFunction(&rbend::LikeMAD );
  else if ( strcasecmp(prop_fun,        "rbend::P_NoEdge" ) == 0 )
    setPropFunction(&rbend::NoEdge );
  else if ( strcasecmp(prop_fun,        "rbend::P_InEdge" ) == 0 )
    setPropFunction(&rbend::InEdge );
  else if ( strcasecmp(prop_fun,        "rbend::P_OutEdge" ) == 0 )
    setPropFunction(&rbend::OutEdge );
  else
    {
      cerr << " **** WARNING **** rbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting rbend::P_Exact\n";
      setPropFunction(&rbend::Exact);
    }
  
  _calcPropParams();

  return is;
}


// **************************************************
//   classes rbend::XXXEdge_Prop
// **************************************************

// --- rbend::NoEdge_Prop -----------------------

rbend::NoEdge_Prop::NoEdge_Prop()
{
}
 
rbend::NoEdge_Prop::~NoEdge_Prop()
{
}
 
// --- rbend::Exact_Prop -----------------------

rbend::Exact_Prop::Exact_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Exact_Prop::~Exact_Prop()
{
}
 
void rbend::Exact_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::InEdge_Prop -----------------------

rbend::InEdge_Prop::InEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::InEdge_Prop::~InEdge_Prop()
{
}
 
void rbend::InEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::OutEdge_Prop -----------------------

rbend::OutEdge_Prop::OutEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::OutEdge_Prop::~OutEdge_Prop()
{
}
 
void rbend::OutEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::Real_Exact_Prop -----------------------

rbend::Real_Exact_Prop::Real_Exact_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Real_Exact_Prop::~Real_Exact_Prop()
{
}
 
void rbend::Real_Exact_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::Real_InEdge_Prop -----------------------

rbend::Real_InEdge_Prop::Real_InEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Real_InEdge_Prop::~Real_InEdge_Prop()
{
}
 
void rbend::Real_InEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- rbend::Real_OutEdge_Prop -----------------------

rbend::Real_OutEdge_Prop::Real_OutEdge_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Real_OutEdge_Prop::~Real_OutEdge_Prop()
{
}
 
void rbend::Real_OutEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}

