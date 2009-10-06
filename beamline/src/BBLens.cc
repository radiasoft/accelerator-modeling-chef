/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BBLens.cc
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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
****** - implemented missing operator=()
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** May 2008           ostiguy@fnal.gov
****** - setStrength() now dispatched to propagator by base class
******   (no longer virtual)
****** - added explicit implementation for assignment operator
****** - templatized field computation
**************************************************************************
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/GenericException.h>
#include <beamline/BBLensPropagators.h>
#include <beamline/BBLens.h>
#include <beamline/BmlVisitor.h>
#include <mxyzptlk/Jet.h>
#include <mxyzptlk/JetVector.h>
#include <algorithm>

using namespace boost;
using namespace std;

static complex<double> complex_i(0.0, 1.0);

extern JetC    w( const JetC& );


// **************************************************
//   class BBLens
// **************************************************

BBLens::BBLens( std::string const&   nm,
                double const&        l, 
                double const&        s,
                double const&        gmm, 
                double const* emt )
  : BmlnElmnt( nm, l, s ), num_(s), gamma_(gmm), sigmas_(3,0.0)
{
  if( ( fabs( gamma_ - 1.0 ) < 0.001 ) || 
      ( gamma_ < 1.0  ) 
    ) {
    cerr << "*** ERROR ***                               \n"
            "*** ERROR *** BBLens::BBLens                \n"
            "*** ERROR *** Gamma is too small            \n"
            "*** ERROR ***                               \n"
         << endl;
  }

  if( emt ) for( int i=0; i<3; ++i) 
    emittance_[i] = (emt[i]/6.0)/sqrt( gmm*gmm - 1.0 );
  else      for( int i=0; i<3; ++i) 
    emittance_[i] = 1.0e-6;

  useRound = 1;

  propagator_ = PropagatorPtr( new Propagator(*this) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens::BBLens( BBLens const& x )
: BmlnElmnt( x ), 
  gamma_(x.gamma_), num_(x.num_),  useRound( x.useRound),  sigmas_(x.sigmas_)
{
  for( int i=0; i<3; ++i ) emittance_[i] = x.emittance_[i];
  for( int i=0; i<3; ++i )     sigmas_[i] = x.sigmas_[i];

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens::~BBLens()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BBLens& BBLens::operator=( BBLens const& rhs) 
{
  if ( &rhs == this) return *this;

  BmlnElmnt::operator=(rhs);

  std::copy( &rhs.emittance_[0], &rhs.emittance_[2]+1,  &emittance_[0]);   // One sigma (noninvariant) emittance_ / pi

  gamma_  = rhs.gamma_;          
  beta_   = rhs.beta_;           
  num_    = rhs.num_;            
  sigmas_ = rhs.sigmas_;

  return *this;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  BBLens::getDistCharge() const
{
  return num_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setDistCharge( double const& N )
{
  num_  = N;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setSigmas( std::vector<double> const& sigmas)
{
  sigmas_ = sigmas;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const&  BBLens::getSigmas() const
{
  return sigmas_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::setDistParameters( double const& npart, double const& Gamma, double const* sigmas )
{
  num_  =  npart;
  gamma_ = Gamma;
  beta_ = sqrt( 1.0 - 1.0 / ( gamma_*gamma_ ) );
  for( int i=0; i<3; ++i) sigmas_[i] = sigmas[i]; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* BBLens::Type() const 
{ 
  return "BBLens"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BBLens::isMagnet() const
{
  return false;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BBLens::isPassive() const
{ 
   return false;  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BBLens::isThin() const
{
   return true;  
}                   

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BBLens::isDriftSpace()  const
{
  return false;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Vector BBLens::Beta() const
{
  static char firstCall = 1;
  static double answer [] = { 0., 0., -1. };
  if( firstCall ) {
    cout << "\n"
            "*** WARNING ***                               \n"
            "*** WARNING *** BBLens::Beta()                \n"
            "*** WARNING *** Needs to be written properly. \n"
            "*** WARNING ***                               \n"
         << endl;
    firstCall = 0;
  }
  return Vector( &answer[0], &answer[0]+3  );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BBLens::accept( ConstBmlVisitor& v ) const  
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
