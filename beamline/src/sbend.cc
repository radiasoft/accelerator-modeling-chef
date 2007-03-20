/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Beamline:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sbend.cc
******  Version:   3.1
******                                                                
******  Author:    Leo Michelotti                                     
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
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - eliminated unneeded dynamic casts in Split(...);
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <basic_toolkit/iosetup.h>
#include <beamline/sbend.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



// **************************************************
//   class sbend
// **************************************************

sbend::sbend() 
: bmlnElmnt( 1.0, 1.0, &sbend::Exact )
  , _angle(0.001)
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(0.0)
  , _dsAngle(0.0)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 _calcPropParams();
}


sbend::sbend( double const& l, double const& s, double const& alpha, PropFunc* pf ) 
: bmlnElmnt( l, s, pf )
  , _angle(alpha)
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(0.0)
  , _dsAngle(0.0)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 if ( fabs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << fabs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( double const& l, double const& s, double const& alpha, PropFunc* pf )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   _angle = - _angle;
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &sbend::Exact;
 }

 _calcPropParams();
}


sbend::sbend( double const& l, double const& s, double const& alpha, 
              double const& us, double const& ds, PropFunc* pf )
:   bmlnElmnt( l, s, pf )
  , _angle(alpha)
  , _usEdgeAngle(us)
  , _dsEdgeAngle(ds)
  , _usAngle(us)
  , _dsAngle(-ds)
  , _usTan(tan(us))
  , _dsTan(-tan(ds))
{
 static bool firstTime = true;
 if ( fabs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << fabs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( double const& l, double const& s, ... )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   _angle = - _angle;
 }
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
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
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
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
   propfunc_ = &sbend::Exact;
 }

 _calcPropParams();
}


sbend::sbend( const char* n, double const& l, double const& s, double const& alpha, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
  , _angle(alpha)
  , _usEdgeAngle(0.0)
  , _dsEdgeAngle(0.0)
  , _usAngle(0.0)
  , _dsAngle(0.0)
  , _usTan(0.0)
  , _dsTan(0.0)
{
 if ( fabs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << fabs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( char* n, double const& l, double const& s, double const& alpha, PropFunc* pf )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   _angle = - _angle;
 }

 // This should never happen, but in case it does.
 if(pf == 0) {
   propfunc_ = &sbend::Exact;
 }

 _calcPropParams();
}


sbend::sbend( const char* n, double const& l, double const& s, double const& alpha, 
              double const& us_arg, double const& ds_arg, PropFunc* pf )
:   bmlnElmnt( n, l, s, pf )
  , _angle(alpha)
  , _usEdgeAngle(us_arg)
  , _dsEdgeAngle(ds_arg)
  , _usAngle(us_arg)
  , _dsAngle(-ds_arg)
  , _usTan(tan(us_arg))
  , _dsTan(-tan(ds_arg))
{

  // Note: us and ds are modified below ...  
  double us = us_arg;
  double ds = ds_arg;
 
 static bool firstTime = true;
 if ( fabs( alpha ) < 1.0e-9 ) {
   ostringstream uic;
   uic  << "| bend angle | = " << fabs(alpha) << " < 1 nanoradian.";
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "sbend::sbend( double const& l, double const& s, ... )",
          uic.str().c_str() ) );
 }
 if( (0.0 > s ) != (0.0 > alpha ) ) {
   (*pcerr) << "\n*** WARNING *** "
           "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
        << "\n*** WARNING *** sbend::sbend( ... )"
           "\n*** WARNING *** Magnetic field and bend angle arguments"
           "\n*** WARNING *** sent to constructor have opposite signs."
           "\n*** WARNING *** Field = " << s << " Tesla"
           "\n*** WARNING *** Bend angle = " << alpha << " radians"
           "\n*** WARNING *** Sign of the bend angle will be changed."
           "\n*** WARNING *** "
        << endl;
   _angle = - _angle;
 }
 if ( (0.0 < fabs(us)) && (fabs( us ) < 1.0e-6) ) {
   us = 0.0;
   _usEdgeAngle = 0.0;
   _usAngle = 0.0;
   _usTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
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
   ds = 0.0;
   _dsEdgeAngle = 0.0;
   _dsAngle = 0.0;
   _dsTan = 0.0;
   if( firstTime) {
     (*pcerr) <<   "*** WARNING *** "
             "\n*** WARNING *** File: " << __FILE__ << ", line " << __LINE__
          << "\n*** WARNING *** sbend::sbend( double const& l, ... PropFunc* pf )"
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
   propfunc_ = &sbend::Exact;
 }

 _calcPropParams();
}


void sbend::_calcPropParams()
{
  // For edge focussing kludge
  _usTan = tan( _usAngle );
  _dsTan = tan( _dsAngle );

  // Geometric parameters
  double psi = _angle - (_usEdgeAngle + _dsEdgeAngle);
  _dphi = - psi;
  _propPhase = std::complex<double> ( cos(psi), sin(psi) );

  double rho = this->Length()/_angle;
  // Note: _angle and rho have the same sign.
  _propTerm =     std::complex<double> ( 0.0, rho )
                * std::complex<double> ( 1.0 - cos(_angle), -sin(_angle) )
                * std::complex<double> ( cos(_dsEdgeAngle), -sin(_dsEdgeAngle) );

  this->setupPropFunc();
}


sbend::sbend( const sbend& x )
:   bmlnElmnt( (bmlnElmnt&) x )
  , _angle(x._angle)
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


sbend::~sbend() 
{
  this->releasePropFunc();
}


double sbend::setEntryAngle( const Particle& p )
{
  return this->setEntryAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double sbend::setExitAngle( const Particle& p )
{
  return this->setExitAngle( atan2( p.get_npx(), p.get_npz() ) );
  // i.e. tan(phi) = px/pz, where pz = longitudinal momentum
}


double sbend::setEntryAngle( double const& phi /* radians */ )
{
  double ret = _usAngle;
  _usAngle = phi;
  _calcPropParams();
  return ret;
}


double sbend::setExitAngle( double const& phi /* radians */ )
{
  double ret = _dsAngle;
  _dsAngle = phi;  
  _calcPropParams();
  return ret;
}


bool sbend::hasParallelFaces() const
{
  return ( std::abs( _usEdgeAngle - _dsEdgeAngle ) <  1.0e-9 );
}


bool sbend::hasStandardFaces() const
{
  return ( (std::abs(_usEdgeAngle) < 1.0e-9) && (std::abs(_dsEdgeAngle) < 0.5e-9) );
}



void sbend::releasePropFunc()
{
  // Nothing needs to be done.
}


void sbend::setupPropFunc()
{
  // Nothing need to be done.
}


bool sbend::isMagnet() const
{
  return true;
}


void sbend::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{

  sbend* sbptr = 0;

  static bool firstTime = true;
  if( firstTime ) {
    firstTime = false;
    (*pcerr) << "\n"
            "\n*** WARNING ***"
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )"
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
         << "\n*** WARNING *** void sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** Splitting sbend with MAD-like propagator."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** You'll get wrong results, "
                              "but it's your fault for using this propagator."
            "\n*** WARNING *** "
         << endl;
     a = SBendPtr( new sbend( pc*length_, strength_,
                    pc*_usAngle,  // this is surely the wrong thing to do
                    propfunc_ ) ); // but THERE IS NO RIGHT THING
     b = SBendPtr( new sbend( (1.0 - pc)*length_, strength_,
                    (1.0 - pc)*_usAngle,
                     propfunc_ ) );
  }
  else if( typeid(*propfunc_) == typeid(NoEdge_Prop) ) {
     a = SBendPtr( new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, propfunc_ ) );
     b = SBendPtr( new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, propfunc_ ) );
  }
  else if( typeid(*propfunc_) == typeid(Exact_Prop) ) {
    a =  SBendPtr ( sbptr = new sbend( pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, &sbend::InEdge ) );
    sbptr->setEntryAngle( this->getEntryAngle() );
    sbptr->setExitAngle( 0.0 );    // Should not matter
    b =  SBendPtr(  sbptr = new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, &sbend::OutEdge ) );
    sbptr->setEntryAngle( 0.0 );   // Should not matter
    sbptr->setExitAngle( this->getExitAngle() );
  }
  else if( typeid(*propfunc_) == typeid(InEdge_Prop) ) {
    a = SBendPtr( sbptr = new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, propfunc_ ) );
    sbptr->setEntryAngle( this->getEntryAngle() );
    sbptr->setExitAngle( 0.0 );    // Should not matter
    b = SBendPtr( new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, &sbend::NoEdge ) );
  }
  else if( typeid(*propfunc_) == typeid(OutEdge_Prop) ) {
    a = SBendPtr( new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, &sbend::NoEdge ) );
    b = SBendPtr( sbptr = new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, propfunc_ ) );
    sbptr->setEntryAngle( 0.0 );   // Should not matter
    sbptr->setExitAngle( this->getExitAngle() );
  }
  // TO BE DONE: else if( typeid(*propfunc_) == typeid(Real_Exact_Prop) ) {
  // TO BE DONE:   *a = new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, &sbend::RealInEdge );
  // TO BE DONE:   *b = new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, &sbend::RealOutEdge );
  // TO BE DONE: }
  // TO BE DONE: else if( typeid(*propfunc_) == typeid(Real_InEdge_Prop) ) {
  // TO BE DONE:   *a = new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, propfunc_ );
  // TO BE DONE:   *b = new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, &sbend::NoEdge );
  // TO BE DONE: }
  // TO BE DONE: else if( typeid(*propfunc_) == typeid(Real_OutEdge_Prop) ) {
  // TO BE DONE:   *a = new sbend(         pc*length_, strength_,         pc*_angle,  _usEdgeAngle, 0.0, &sbend::NoEdge );
  // TO BE DONE:   *b = new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle,  0.0, _dsEdgeAngle, propfunc_ );
  // TO BE DONE: }
  else {
    (*pcerr) << "\n*** WARNING *** "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void sbend::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** Propagator type unrecognized."
            "\n*** WARNING *** I'm not responsible for what happens."
            "\n*** WARNING *** It's all your fault."
            "\n*** WARNING *** "
         << endl;
    a = SBendPtr( new sbend( pc*length_,         strength_,         pc*_angle, _usEdgeAngle, 0.0 ) );
    b = SBendPtr( new sbend( (1.0 - pc)*length_, strength_, (1.0 - pc)*_angle, 0.0, _dsEdgeAngle ) );
  }

  // Rename

  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}


ostream& sbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << _angle << " "
     << OSTREAM_DOUBLE_PREC << _usEdgeAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsEdgeAngle << endl;
  os << OSTREAM_DOUBLE_PREC << _usAngle << " "
     << OSTREAM_DOUBLE_PREC << _dsAngle << endl;

  // Determine which propogators are being used, and make a note of it.
  if ( propfunc_ ==            &sbend::Exact )
    os << "sbend::P_Exact    sbend::J_Exact";
  else if ( propfunc_ ==       &sbend::LikeMAD )
    os << "sbend::P_LikeMAD  sbend::J_LikeMAD";
  else if ( propfunc_ ==       &sbend::NoEdge )
    os << "sbend::P_NoEdge   sbend::J_NoEdge";
  else if ( propfunc_ ==       &sbend::InEdge )
    os << "sbend::P_InEdge   sbend::J_InEdge";
  else if ( propfunc_ ==       &sbend::OutEdge )
    os << "sbend::P_OutEdge  sbend::J_OutEdge";
  else
    os << "UNKNOWN  UNKNOWN";
  
  os << "\n";
  return os;
}


istream& sbend::readFrom(istream& is)
{
  is >> _angle >> _usEdgeAngle >> _dsEdgeAngle;
  is >> _usAngle >> _dsAngle;

  char prop_fun[100], jet_prop_fun[100];
  is >> prop_fun >> jet_prop_fun;

  if ( strcasecmp(prop_fun,             "sbend::P_Exact" ) == 0 )
    setPropFunction(&sbend::Exact );
  else if ( strcasecmp(prop_fun,        "sbend::P_LikeMAD" ) == 0 )
    setPropFunction(&sbend::LikeMAD );
  else if ( strcasecmp(prop_fun,        "sbend::P_NoEdge" ) == 0 )
    setPropFunction(&sbend::NoEdge );
  else if ( strcasecmp(prop_fun,        "sbend::P_InEdge" ) == 0 )
    setPropFunction(&sbend::InEdge );
  else if ( strcasecmp(prop_fun,        "sbend::P_OutEdge" ) == 0 )
    setPropFunction(&sbend::OutEdge );
  else
    {
      (*pcerr) << " **** WARNING **** sbend::readFrom(istream)\n";
      (*pcerr) << " **** WARNING **** unknown propagator function specified:\n";
      (*pcerr) << " **** WARNING **** " << prop_fun << "\n";
      (*pcerr) << " **** WARNING **** Substituting sbend::P_Exact\n";
      setPropFunction(&sbend::Exact);
    }
  
  _calcPropParams();

  return is;
}


// **************************************************
//   Frame functions
// **************************************************

void sbend::enterLocalFrame( Particle& p ) const
{

  bmlnElmnt::enterLocalFrame(p);

//   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
//        "void sbend::enterLocalFrame( Particle& p ) const",
//        "Function is obsolete. (?)" ) );
// TO BE DONE   static double halfAngle;
// TO BE DONE   halfAngle = this->_angle / 2.0;
// TO BE DONE   P_Face( p,   halfAngle );
// TO BE DONE   bmlnElmnt::enterLocalFrame( p );
// TO BE DONE   P_Face( p, - halfAngle );
}


void sbend::enterLocalFrame( JetParticle& p ) const
{

 bmlnElmnt::enterLocalFrame(p);

// throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
//      "void sbend::enterLocalFrame( JetParticle& p ) const",
//     "Function is obsolete. (?)" ) );
// TO BE DONE   static double halfAngle;
// TO BE DONE   halfAngle = this->_angle / 2.0;
// TO BE DONE   J_Face( p,   halfAngle );
// TO BE DONE   bmlnElmnt::enterLocalFrame( p );
// TO BE DONE   J_Face( p, - halfAngle );
}


void sbend::leaveLocalFrame( Particle& p ) const
{

  bmlnElmnt::leaveLocalFrame(p);

//   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
//        "void sbend::leaveLocalFrame( Particle& p ) const",
//        "Function is obsolete. (?)" ) );
// TO BE DONE   static double halfAngle;
// TO BE DONE   halfAngle = this->_angle / 2.0;
// TO BE DONE   P_Face( p, - halfAngle );
// TO BE DONE   bmlnElmnt::leaveLocalFrame( p );
// TO BE DONE   P_Face( p,   halfAngle );
}


void sbend::leaveLocalFrame( JetParticle& p ) const
{
  bmlnElmnt::leaveLocalFrame(p);


//   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
//        "void sbend::leaveLocalFrame( JetParticle& p ) const",
//        "Function is obsolete. (?)" ) );
// TO BE DONE   static double halfAngle;
// TO BE DONE   halfAngle = this->_angle / 2.0;
// TO BE DONE   J_Face( p, - halfAngle );
// TO BE DONE   bmlnElmnt::leaveLocalFrame( p );
// TO BE DONE   J_Face( p,   halfAngle );
}


// TO BE DONE void sbend::P_Face( Particle& p, const double& psi ) const
// TO BE DONE {
// TO BE DONE   static int x;
// TO BE DONE   static int y;
// TO BE DONE   static int cdt;
// TO BE DONE   static int xp;
// TO BE DONE   static int yp;
// TO BE DONE   static int dpop;
// TO BE DONE 
// TO BE DONE   x    = p.xIndex();
// TO BE DONE   y    = p.yIndex();
// TO BE DONE   cdt  = p.cdtIndex();
// TO BE DONE   xp   = p.npxIndex();
// TO BE DONE   yp   = p.npyIndex();
// TO BE DONE   dpop = p.ndpIndex();
// TO BE DONE 
// TO BE DONE 
// TO BE DONE   double cs( cos(psi) ); 
// TO BE DONE   double sn( sin(psi) ); 
// TO BE DONE 
// TO BE DONE   // Drift frame represented in the sbend frame.
// TO BE DONE   Vector e_1(3), e_2(3), e_3(3);
// TO BE DONE   e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
// TO BE DONE   e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
// TO BE DONE   e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 
// TO BE DONE 
// TO BE DONE   // Coordinate transformation.
// TO BE DONE   Vector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
// TO BE DONE   Vector dummy    ( p.VectorBeta() );
// TO BE DONE   Vector beta     ( dummy(0)*e_1 +
// TO BE DONE                     dummy(1)*e_2 +
// TO BE DONE                     dummy(2)*e_3 );
// TO BE DONE 
// TO BE DONE   double tau      ( - r(2) / beta(2) );
// TO BE DONE 
// TO BE DONE   p.state[x]    = r(0) + tau*beta(0);
// TO BE DONE   p.state[y]    = r(1) + tau*beta(1);
// TO BE DONE   p.state[cdt] += tau;
// TO BE DONE 
// TO BE DONE   // Momentum transformation
// TO BE DONE   double p1( p.State( xp ) );
// TO BE DONE   double p2( p.State( yp ) );
// TO BE DONE   double p3divpbar = sqrt( ( 1.0 + p.state[dpop] ) * ( 1.0 + p.state[dpop] )
// TO BE DONE                             - p1*p1 - p2*p2 );
// TO BE DONE 
// TO BE DONE   p.state[xp] = cs*p.State( xp ) + sn*p3divpbar;
// TO BE DONE }


// TO BE DONE void sbend::J_Face( JetParticle& p, const double& psi ) const
// TO BE DONE {
// TO BE DONE   static int x;
// TO BE DONE   static int y;
// TO BE DONE   static int cdt;
// TO BE DONE   static int xp;
// TO BE DONE   static int yp;
// TO BE DONE   static int dpop;
// TO BE DONE 
// TO BE DONE   x    = p.xIndex();
// TO BE DONE   y    = p.yIndex();
// TO BE DONE   cdt  = p.cdtIndex();
// TO BE DONE   xp   = p.npxIndex();
// TO BE DONE   yp   = p.npyIndex();
// TO BE DONE   dpop = p.ndpIndex();
// TO BE DONE 
// TO BE DONE 
// TO BE DONE   double cs( cos(psi) ); 
// TO BE DONE   double sn( sin(psi) ); 
// TO BE DONE 
// TO BE DONE   // Drift frame represented in the sbend frame.
// TO BE DONE   Vector e_1(3), e_2(3), e_3(3);
// TO BE DONE   e_1(0) = cs;  e_1(1) = 0.0;  e_1(2) = -sn; 
// TO BE DONE   e_2(0) = 0.0; e_2(1) = 1.0;  e_2(2) = 0.0; 
// TO BE DONE   e_3(0) = sn;  e_3(1) = 0.0;  e_3(2) = cs; 
// TO BE DONE 
// TO BE DONE   // Coordinate transformation.
// TO BE DONE   JetVector r        ( p.State(x)*e_1 + p.State(y)*e_2 );
// TO BE DONE   JetVector dummy    ( p.VectorBeta() );
// TO BE DONE   JetVector beta     ( dummy(0)*e_1 +
// TO BE DONE                        dummy(1)*e_2 +
// TO BE DONE                        dummy(2)*e_3 );
// TO BE DONE 
// TO BE DONE   Jet tau            ( - r(2) / beta(2) );
// TO BE DONE 
// TO BE DONE  ( p.state ).SetComponent( x,   r(0) + tau*beta(0) );
// TO BE DONE  ( p.state ).SetComponent( y,   r(1) + tau*beta(1) );
// TO BE DONE  ( p.state ).SetComponent( cdt, p.state(cdt) + tau );
// TO BE DONE 
// TO BE DONE   // Momentum transformation
// TO BE DONE   Jet p1( p.State( xp ) );
// TO BE DONE   Jet p2( p.State( yp ) );
// TO BE DONE   Jet p3divpbar = sqrt( ( 1.0 + p.state(dpop) ) * ( 1.0 + p.state(dpop) )
// TO BE DONE                             - p1*p1 - p2*p2 );
// TO BE DONE 
// TO BE DONE   ( p.state ).SetComponent( xp, cs*p.State( xp ) + sn*p3divpbar );
// TO BE DONE }


const char* sbend::Type() const 
{ 
  return "sbend"; 
}

void sbend::accept( BmlVisitor& v ) 
{ 
 v.visit( *this ); 
}

void sbend::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}


// **************************************************
//   classes sbend::XXXEdge_Prop
// **************************************************

// --- rbend::NoEdge_Prop -----------------------

sbend::NoEdge_Prop::NoEdge_Prop()
{
}
 
sbend::NoEdge_Prop::~NoEdge_Prop()
{
}
 
// --- sbend::Exact_Prop -----------------------

sbend::Exact_Prop::Exact_Prop()
{
  _myPropagator = &sbend::NoEdge;
}
 
sbend::Exact_Prop::~Exact_Prop()
{
}
 
void sbend::Exact_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- sbend::InEdge_Prop -----------------------

sbend::InEdge_Prop::InEdge_Prop()
{
  _myPropagator = &sbend::NoEdge;
}
 
sbend::InEdge_Prop::~InEdge_Prop()
{
}
 
void sbend::InEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}


// --- sbend::OutEdge_Prop -----------------------

sbend::OutEdge_Prop::OutEdge_Prop()
{
  _myPropagator = &sbend::NoEdge;
}
 
sbend::OutEdge_Prop::~OutEdge_Prop()
{
}
 
void sbend::OutEdge_Prop::setPropagator( NoEdge_Prop* x )
{
  _myPropagator = x;
}



