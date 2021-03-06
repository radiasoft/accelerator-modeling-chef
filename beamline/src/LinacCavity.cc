/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LinacCavity.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                                                   
******
****** REVISION HISTORY
******
****** May 2008   ostiguy@fnal.gov
****** - setStrength() /setLength () now dispatched to propagator 
******   by base class (no longer virtual)
****** - added explicit implementation for assignment operator
****** Apr 2008   michelotti@fnal.gov
****** - added placeholder LinacCavity::setLength method [ now eliminated -jfo]
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagator architecture
******
******
**************************************************************************
*************************************************************************/

//=======================================================================
//
// PHASE SIGN CONVENTION IN CAVITIES 
//
// In CHEF, cdt > 0  implies that a particle is **late** w/r to the synchronous 
// particle. In a synchrotron, the phase 0 is chosen to correspond to a stationary 
// bucket. 
// 
// Volts = sin(  phase + cdt )
//
// Below transition, phase stability is achieved if particles arriving later (i.e. the 
// slowest ones, with smaller momomenta) are given a larger kick. 
// Above transition, the reverse applies i.e. phase stability requires phase = pi 
// in that case.
//  
// By convention, in a Linac, the acceleration is chosen to be ****maximum**** when 
// phase = 0.0 
//
// So, assuming the following form to compute the kick
// 
// Volts = cos (  phase + cdt )                   
// 
// one can observe that for phase > 0, the RF crest precedes a particle with cdt=0 
// i.e phase > 0 causes the (unloaded) voltage to be greater for the head of a bunch 
// than for its tail (the correct sign for BNS damping). 
//
//			  
//          * *	                              			  	  
//        *     * 	  		                * *		  
//       OTail   *	  		              *     * 	  
//      O         *	  		             *       O Head	  
//     O           *	  		            *         O	  
//    *Head         *	  		           *           O Tail	  
//   *               *	  		          *             *	  
//  *                 *	  		         *               *	  
//			  		        *                 *	  
//    Bunch Phase < 0     		      			  
//					          Bunch Phase > 0     
//  <<=== increasing phase 
//        increasing cdt ====>  
//
// Conversely, phase < 0 causes the (unloaded) voltage to be greater for the tail 
// of a bunch than for its head  (the correct sign for beam loading compensation).  
//
// 
//=====================================================================================

#include <iomanip>
#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/LinacCavityPropagators.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/BmlVisitor.h>
#include <beamline/marker.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LinacCavity::LinacCavity( std::string const& name,
                          double const& length,     // length [m]
                          double const& f,          // rf frequency 
                          double const& eV,         // rf voltage 
                          double const& phi_s,      // synchronous phase 
                          bool   wake_on     )      
: BmlnElmnt( name, length,  eV ), w_rf_(2*M_PI*f), phi_s_(phi_s), wakeon_(wake_on)

{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::LinacCavity( LinacCavity const& x ) 
  : BmlnElmnt( x ), w_rf_(x.w_rf_), phi_s_(x.phi_s_), wakeon_( x.wakeon_)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::~LinacCavity()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity& LinacCavity::operator=(LinacCavity const& rhs)
{
  if (this == &rhs ) return *this;
  
  BmlnElmnt::operator=(rhs);

  w_rf_  = rhs.w_rf_;
  phi_s_ = rhs.phi_s_;

  return *this;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& LinacCavity::writeTo(ostream& os) 
{
#if 0  
os << OSTREAM_DOUBLE_PREC 
     << " " << getRadialFrequency()/ MATH_TWOPI
     << " " << getPhi()
     << '\n';
#endif
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& LinacCavity::readFrom(istream& is) 
{
  double w, phi_s;
  is >> w_rf_
     >> phi_s_; 

  return is;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* LinacCavity::Type() const 
{
  return "LinacCavity"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool    LinacCavity::isMagnet() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
bool    LinacCavity::isThin() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
bool    LinacCavity::isPassive() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
bool    LinacCavity::isDriftSpace() const
{
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::accept( BmlVisitor& v ) 
{ 
  v.visit( *this); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LinacCavity::phi()  const
{
  return phi_s_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double LinacCavity::designEnergyGain()   const
{
  return Strength() * cos(phi_s_); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   LinacCavity::setFrequency( double const& freq)
{
  w_rf_ = 2.0*M_PI*freq;

  propagator_->setAttribute( *this, "FREQUENCY", freq );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double   LinacCavity::frequency() const
{
  return w_rf_/(2.0*M_PI);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& LinacCavity::radialFrequency() const
{
  return w_rf_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void  LinacCavity::setPhi( double const& radians)
{
  phi_s_ = radians;
  propagator_->setAttribute(*this, "PHASE", radians );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void  LinacCavity::setWakeOn( bool set )
{
  wakeon_ = set;
  propagator_->setAttribute(*this, "WAKEON", set);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

bool LinacCavity::wakeOn() const
{
  return wakeon_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr> LinacCavity::split( double const& pc ) const
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: "  << __FILE__ << "," << __LINE__
           << "\n*** WARNING ****: void " << Type() << "::split( double const& ) const"
              "\n*** WARNING ****: Splitting a " << Type() << " is forbidden in this version."
              "\n*** WARNING ****: " 
           << std::endl;
  ostringstream uic;
  uic  <<   "Splitting a " << Type() << " is forbidden in this version.";
  throw( GenericException( __FILE__, __LINE__, 
         " LinacCavity::split( double const& ) const", 
         uic.str().c_str() ) );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   LinacCavity::propagateReference( Particle& particle, double initialBRho, bool scaling )
{
  propagator_->propagateReference( particle, initialBRho, scaling );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
