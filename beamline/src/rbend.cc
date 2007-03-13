/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      rbend.cc
******  Version:   3.2
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - eliminated unecessary dynamic casts in Split(...);
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <typeinfo>

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/PhysicsConstants.h>
#include <beamline/rbend.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/BmlVisitor.h>

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;
;

// **************************************************
//   class rbend
// **************************************************

rbend::rbend()
: bmlnElmnt( 1.0, 0.0, &rbend::Exact )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 _calcPropParams();
}


rbend::rbend( double const& l, double const& s, PropFunc* pf ) 
: bmlnElmnt( l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double const& l, double const& s, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( double const& l, double const& s, double const& entryangle, PropFunc* pf ) 
: bmlnElmnt( l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(entryangle)
  , _dsAngle(-entryangle)
  , _usTan(tan(entryangle))
  , _dsTan(-tan(entryangle))
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-9) ) {
   _usAngle = 0.0;
   _usTan   = 0.0;
   _dsAngle = 0.0;
   _dsTan   = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double const& l, double const& s, double const& entryangle, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(entryangle)
  , _dsAngle(-entryangle)
  , _usTan(tan(entryangle))
  , _dsTan(-tan(entryangle))
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-9) ) {
   _usAngle = 0.0;
   _usTan   = 0.0;
   _dsAngle = 0.0;
   _dsTan   = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( double const& l, double const& s, double const& us, double const& ds, PropFunc* pf )
:   bmlnElmnt( l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 static bool firstTime = true;
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = 0.0;
   _dsTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double const& l, double const& s, double const& us, double const& ds, PropFunc* pf )
:   bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(M_TWOPI)
  , _dsAngle(-M_TWOPI)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 static bool firstTime = true;
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = 0.0;
   _dsTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( double const& l, double const& s, double const& entryangle, double const& us, double const& ds, PropFunc* pf )
:   bmlnElmnt( l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(entryangle + us)
  , _dsAngle(-(entryangle + ds))
  , _usTan(tan(entryangle + us))
  , _dsTan(-tan(entryangle + ds))
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-9) ) {
   _usAngle = us;
   _usTan   = tan(us);
   _dsAngle = -ds;
   _dsTan   = -tan(ds);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   _usEdgeAngle = 0.0;
   _usAngle = entryangle;
   _usTan = tan(entryangle);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = -entryangle;
   _dsTan = -tan(entryangle);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
 }

 _calcPropParams();
}


rbend::rbend( const char* n, double const& l, double const& s, double const& entryangle, double const& us, double const& ds, PropFunc* pf )
:   bmlnElmnt( n, l, s, pf )
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(entryangle + us)
  , _dsAngle(-(entryangle + ds))
  , _usTan(tan(entryangle + us))
  , _dsTan(-tan(entryangle + ds))
{
 static bool firstTime = true;
 if( (0.0 < fabs(entryangle)) && (fabs( entryangle) < 1.0e-9) ) {
   _usAngle = us;
   _usTan   = tan(us);
   _dsAngle = -ds;
   _dsTan   = -tan(ds);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( char* n, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream entry angle | = " 
          << fabs(entryangle) 
          << " < 1 nanoradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   _usEdgeAngle = 0.0;
   _usAngle = entryangle;
   _usTan = tan(entryangle);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | upstream edge angle | = " 
          << fabs(us) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }
 if ( (0.0 < fabs(ds)) && (fabs( ds ) < 1.0e-6) ) {
   _dsEdgeAngle = 0.0;
   _dsAngle = -entryangle;
   _dsTan = -tan(entryangle);
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** rbend::rbend( double const& l, ... PropFunc* pf )"
             "\n*** WARNING *** | downstream edge angle | = " 
          << fabs(ds) 
          << " < 1 microradian."
             "\n*** WARNING *** It will be reset to zero."
             "\n*** WARNING *** This message is written once only."
          << endl;
     firstTime = false;
   }
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &rbend::Exact;
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
  _dphi = - psi;
  _propPhase = std::complex<double> ( cos(psi), sin(psi) );

  _propTerm =   this->Length()
              * std::complex<double> ( cos(_dsEdgeAngle), -sin(_dsEdgeAngle) );

  this->setupPropFunc();
}


rbend::rbend( const rbend& x )
: bmlnElmnt(x )
  , _usEdgeAngle(x._usEdgeAngle)
  , _dsEdgeAngle(x._dsEdgeAngle)
  , _usAngle(x._usAngle)
  , _dsAngle(x._dsAngle)
  , _usTan(x._usTan)
  , _dsTan(x._dsTan)
  , _dphi(x._dphi)
  , _propPhase(x._propPhase)
  , _propTerm(x._propTerm)
{
  this->setupPropFunc();
}


rbend::~rbend() 
{
  this->releasePropFunc();
}


void rbend::releasePropFunc()
{
  // Nothing needs to be done.
}


void rbend::setupPropFunc()
{
  // Nothing need to be done.
}


const char* rbend::Type() const { 
  return "rbend"; 
}


double rbend::OrbitLength( const Particle& x )
{
  // Computes arclength of orbit assuming a symmetric bend.
  // WARNING: This is not the true arclength under all conditions.
  double tworho = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength_;
  return tworho * asin( length_ / tworho );
}


bool rbend::isMagnet() const
{
  return true;
}


void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{

  rbend* p = 0;

  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** The new, split elements must be commissioned with"
            "\n*** WARNING *** RefRegVisitor before being used."
            "\n*** WARNING *** "
         << endl;
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  if( typeid(*propfunc_) == typeid(MAD_Prop) ) {
    (*pcerr) << "\n*** WARNING *** "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double const& pc, ElmPtr& a, ElmPtr& b )"
            "\n*** WARNING *** Splitting rbend with MAD-like propagator."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** You'll get wrong results, "
                              "but it's your fault for using this propagator."
            "\n*** WARNING *** "
         << endl;
    // NOTE: pc*_usAngle is surely the wrong thing to do but there is no right thing
    a = ElmPtr( new rbend( pc*length_, strength_, pc*_usAngle, propfunc_ ) );     
    b = ElmPtr( new rbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_usAngle, propfunc_ ) );
  }
  else if( typeid(*propfunc_) == typeid(NoEdge_Prop) ) {
    a = ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, propfunc_ ) );
    b = ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, propfunc_ ) );
  }
  else if( typeid(*propfunc_) == typeid(Exact_Prop) ) {
    a =  ElmPtr( p = new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, &rbend::InEdge ));
    p->setEntryAngle( this->getEntryAngle() );
    p->setExitAngle( 0.0 );    // Should not matter
    b =  ElmPtr( p = new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, &rbend::OutEdge ));
    p->setEntryAngle( 0.0 );   // Should not matter
    p->setExitAngle( this->getExitAngle() );
  }
  else if( typeid(*propfunc_) == typeid(InEdge_Prop) ) {
    a = ElmPtr( p = new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, propfunc_ ) );
    p->setEntryAngle( getEntryAngle() );
    p->setExitAngle( 0.0 );    // Should not matter
    b =  ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, &rbend::NoEdge ) );
  }
  else if( typeid(*propfunc_) == typeid(OutEdge_Prop) ) {
    a =  ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, &rbend::NoEdge ) );
    b =  ElmPtr( p = new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, propfunc_ ) );
    p->setEntryAngle( 0.0 );   // Should not matter
    p->setExitAngle( this->getExitAngle() );
  }
  else if( typeid(*propfunc_) == typeid(Real_Exact_Prop) ) {
    a = ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, &rbend::RealInEdge )  );
    b = ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, &rbend::RealOutEdge ) );
  }
  else if( typeid(*propfunc_) == typeid(Real_InEdge_Prop) ) {
    a = ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, propfunc_ ) );
    b = ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, &rbend::NoEdge ) );
  }
  else if( typeid(*propfunc_) == typeid(Real_OutEdge_Prop) ) {
    a =  ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, &rbend::NoEdge ) );
    b =  ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, propfunc_ ) );
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** Propagator type unrecognized."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** It's all your fault."
            "\n*** WARNING *** "
         << endl;
    a =  ElmPtr( new rbend(         pc*length_, strength_, _usEdgeAngle, 0.0, &rbend::NoEdge ) );
    b =  ElmPtr( new rbend( (1.0 - pc)*length_, strength_, 0.0, _dsEdgeAngle, &rbend::NoEdge ) );
  }

  // Rename

  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}


double rbend::setPoleFaceAngle( const Particle& p )
{
  double psi =   
     asin(   ( this->strength_ * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );
  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)
  //                  = 1/2 symmetric bend angle
  setEntryAngle( psi );
  setExitAngle( -psi );
  return _usAngle;
}


double rbend::setPoleFaceAngle( JetParticle const& p )
{
  double psi =   
     asin(   ( this->strength_ * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );
  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)
  //                  = 1/2 symmetric bend angle
  setEntryAngle( psi );
  setExitAngle( -psi );
  return _usAngle;
}


double rbend::setEntryAngle( const Particle& p )
{
  return setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double rbend::setExitAngle( const Particle& p )
{
  return setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double rbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = _usAngle;
  _usAngle = phi;
  _calcPropParams();
  return ret;
}


double rbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = _dsAngle;
  _dsAngle = phi;  
  _calcPropParams();
  return ret;
}


bool rbend::hasParallelFaces() const
{
  return ( std::abs( _usEdgeAngle + _dsEdgeAngle ) <  1.0e-9 );
}


bool rbend::hasStandardFaces() const
{
  return ( (std::abs(_usEdgeAngle) < 1.0e-9) && (std::abs(_dsEdgeAngle) < 0.5e-9) );
}


ostream& rbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << _usEdgeAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsEdgeAngle << endl;
  os << OSTREAM_DOUBLE_PREC << _usAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsAngle << endl;

  // Determine which propogators are being used, and make a note of it.
  if ( propfunc_ ==            &rbend::Exact )
    os << "rbend::P_Exact    rbend::J_Exact";
  else if ( propfunc_ ==       &rbend::LikeMAD )
    os << "rbend::P_LikeMAD  rbend::J_LikeMAD";
  else if ( propfunc_ ==       &rbend::NoEdge )
    os << "rbend::P_NoEdge   rbend::J_NoEdge";
  else if ( propfunc_ ==       &rbend::InEdge )
    os << "rbend::P_InEdge   rbend::J_InEdge";
  else if ( propfunc_ ==       &rbend::OutEdge )
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
      (*pcerr) << " **** WARNING **** rbend::readFrom(istream)\n";
      (*pcerr) << " **** WARNING **** unknown propagator function specified:\n";
      (*pcerr) << " **** WARNING **** " << prop_fun << "\n";
      (*pcerr) << " **** WARNING **** Substituting rbend::P_Exact\n";
      setPropFunction(&rbend::Exact);
    }
  
  _calcPropParams();

  return is;
}

void rbend::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

void rbend::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit(*this ); 
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

