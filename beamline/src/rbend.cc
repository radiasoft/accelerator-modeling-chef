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
: bmlnElmnt( l, s, pf ), _CT(0.0) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}

rbend::rbend( double l, double s, double angle, PropFunc* pf ) 
: bmlnElmnt( l, s, pf ), _CT(0.0) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( char* n, double l, double s, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf ), _CT(0.0) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = M_TWOPI;
 tanPFAngle = 0.0;
}


rbend::rbend( char* n, double l, double s, double angle, PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf ), _CT(0.0) {
 if(pf == 0) {
   Propagator = &rbend::Exact;
 }
 poleFaceAngle = angle;
 tanPFAngle    = tan( poleFaceAngle );
}


rbend::rbend( const rbend& x )
: bmlnElmnt( (bmlnElmnt&) x ), _CT(0.0)
{
  poleFaceAngle = x.poleFaceAngle;
  tanPFAngle    = tan( poleFaceAngle );
}


rbend::~rbend() {
}


void rbend::eliminate() {
 delete this;
}


const char* rbend::Type() const { 
  return "rbend"; 
}


double rbend::OrbitLength( const Particle& x )
{
  static double tworho;
  tworho  = 2.0 * ( x.Momentum() / PH_CNV_brho_to_p ) / strength;
  return tworho * asin( length / tworho );
}


void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    cerr << "\n"
            "\n*** WARNING ***                                    "
            "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** void rbend::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )"
            "\n*** WARNING *** pc = " << pc << 
                                        " is out of bounds.       "
            "\n*** WARNING ***                                    "
         << endl;
    *a = 0;
    *b = 0;
    return;
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
                    pc*poleFaceAngle, // this is surely the wrong thing to do
                    Propagator );     // but there is no right thing
    *b = new rbend( (1.0 - pc)*length, strength, 
                    (1.0 - pc)*poleFaceAngle, 
                    Propagator );
  }
  else if( typeid(*Propagator) == typeid(NoEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, Propagator );
  }
  else if( typeid(*Propagator) == typeid(Exact_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, &rbend::InEdge );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, &rbend::OutEdge );
  }
  else if( typeid(*Propagator) == typeid(InEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, &rbend::NoEdge );
  }
  else if( typeid(*Propagator) == typeid(OutEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, &rbend::NoEdge );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, Propagator );
  }
  else if( typeid(*Propagator) == typeid(Real_Exact_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, &rbend::RealInEdge );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, &rbend::RealOutEdge );
  }
  else if( typeid(*Propagator) == typeid(Real_InEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, Propagator );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, &rbend::NoEdge );
  }
  else if( typeid(*Propagator) == typeid(Real_OutEdge_Prop) ) {
    *a = new rbend(         pc*length, strength, poleFaceAngle, &rbend::NoEdge );
    *b = new rbend( (1.0 - pc)*length, strength, poleFaceAngle, Propagator );
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
    *a = new rbend( pc*length, strength );
    *b = new rbend( (1.0 - pc)*length, strength );
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
  this->poleFaceAngle =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );

  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)

  this->tanPFAngle = tan( this->poleFaceAngle );
  return this->poleFaceAngle;
}


double rbend::setPoleFaceAngle( const JetParticle& p )
{
  this->poleFaceAngle =   
     asin(   ( this->strength * this->Length() )
           / ( 2.0*p.ReferenceBRho() )
         );

  // i.e., sin( psi ) = (l/2) / rho
  //                  = Bl/(2*Brho)

  this->tanPFAngle = tan( this->poleFaceAngle );
  return this->poleFaceAngle;
}


double rbend::setReferenceTime( const Particle& prtn )
{
  // Assumes "normal" symmetric crossing.
  static const double csq_red = PH_MKS_c * PH_MKS_c * 1.0e-9;
  double Omega  = csq_red * this->Strength() / prtn.ReferenceEnergy();
  double Rho  = prtn.ReferenceBRho() / this->Strength();
  _CT = PH_MKS_c * ( 2.0*asin( this->Length() / (2.0*Rho) ) ) / Omega;
  return _CT;
}


double rbend::setReferenceTime( double x )
{
  double oldValue = _CT;
  _CT = x;
  if( fabs(_CT) < 1.0e-12 ) { _CT = 0.0; }
  return oldValue;
}


ostream& rbend::writeTo(ostream& os)
{
  os << OSTREAM_DOUBLE_PREC << poleFaceAngle << " ";
  // Determine which propogators are being used, and make a note of it.
  if ( Propagator == 		&rbend::Exact )
    os << "rbend::P_Exact    rbend::J_Exact";
  else if ( Propagator == 	&rbend::LikeMAD )
    os << "rbend::P_LikeMAD  rbend::J_LikeMAD";
  else if ( Propagator == 	&rbend::NoEdge )
    os << "rbend::P_NoEdge   rbend::J_NoEdge";
  else if ( Propagator == 	&rbend::InEdge )
    os << "rbend::P_InEdge   rbend::J_InEdge";
  else if ( Propagator == 	&rbend::OutEdge )
    os << "rbend::P_OutEdge  rbend::J_OutEdge";
  else
    os << "UNKNOWN  UNKNOWN";
  
  os << "\n";
  return os;
}

istream& rbend::readFrom(istream& is)
{
  double a;
  char prop_fun[100], jet_prop_fun[100];

  is >> a >> prop_fun >> jet_prop_fun;
  setAngle(a);

  if ( strcasecmp(prop_fun, 		"rbend::P_Exact" ) == 0 )
    setPropFunction(&rbend::Exact );
  else if ( strcasecmp(prop_fun, 	"rbend::P_LikeMAD" ) == 0 )
    setPropFunction(&rbend::LikeMAD );
  else if ( strcasecmp(prop_fun, 	"rbend::P_NoEdge" ) == 0 )
    setPropFunction(&rbend::NoEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_InEdge" ) == 0 )
    setPropFunction(&rbend::InEdge );
  else if ( strcasecmp(prop_fun, 	"rbend::P_OutEdge" ) == 0 )
    setPropFunction(&rbend::OutEdge );
  else
    {
      cerr << " **** WARNING **** rbend::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting rbend::P_Exact\n";
      setPropFunction(&rbend::Exact);
    }
  
  return is;
}


// **************************************************
//   classes rbend::XXXEdge_Prop
// **************************************************

// --- rbend::NoEdge_Prop -----------------------

rbend::NoEdge_Prop::NoEdge_Prop()
: _approx(0)
{
}
 
rbend::NoEdge_Prop::~NoEdge_Prop()
{
}
 
char rbend::NoEdge_Prop::isApproximate()
{
  return _approx;
}

void rbend::NoEdge_Prop::makeApproximate()
{
  _approx = 1;
}

void rbend::NoEdge_Prop::makeExact()
{
  _approx = 0;
}


// --- rbend::Exact_Prop -----------------------

rbend::Exact_Prop::Exact_Prop()
{
  _myPropagator = &rbend::NoEdge;
}
 
rbend::Exact_Prop::~Exact_Prop()
{
}
 
char rbend::Exact_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::Exact_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::Exact_Prop::makeExact()
{
  _myPropagator->makeExact();
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
 
char rbend::InEdge_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::InEdge_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::InEdge_Prop::makeExact()
{
  _myPropagator->makeExact();
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
 
char rbend::OutEdge_Prop::isApproximate()
{
  return _myPropagator->isApproximate();
}

void rbend::OutEdge_Prop::makeApproximate()
{
  _myPropagator->makeApproximate();
}

void rbend::OutEdge_Prop::makeExact()
{
  _myPropagator->makeExact();
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


