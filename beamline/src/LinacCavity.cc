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
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                                                   
******
****** REVISION HISTORY
******
****** Dec 2007   ostiguy@fnal.gov
****** - new typesafe propagator architecture
******
**************************************************************************
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
#include <beamline/beamline.h>
#include <beamline/LinacCavity.h>
#include <beamline/LinacCavityPropagators.h>
#include <beamline/LinacCavityParts.h>
#include <beamline/WakeKick.h>
#include <beamline/WakeKickPropagator.h>
#include <beamline/BmlVisitor.h>
#include <beamline/RefRegVisitor.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LinacCavity::LinacCavity( const char* name,         // name
                          double const& length,     // length [m]
                          double const& f,          // rf frequency 
                          double const& eV,         // rf voltage 
                          double const& phi_s,      // synchronous phase 
                          bool   wake_on     )      
 : bmlnElmnt( name, length,  eV*1.0e-9), w_rf_(2*M_PI*f), phi_s_(phi_s)

{
  propagator_ = PropagatorPtr(new Propagator() );
  propagator_->setup(*this);

  setWakeOn( wake_on);

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::LinacCavity( LinacCavity const& x ) 
 : bmlnElmnt( x ), w_rf_(x.w_rf_), phi_s_(x.phi_s_),
   propagator_( x.propagator_->Clone() )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::~LinacCavity()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& LinacCavity::writeTo(ostream& os) 
{
  os << OSTREAM_DOUBLE_PREC 
     << " " << getRadialFrequency()/ MATH_TWOPI
     << " " << getPhi()
     << '\n';
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

double LinacCavity::getReferenceTime() const 
{

  ctRef_ = 0.0;

  for ( beamline::const_iterator it  = bml_->begin(); 
                                 it != bml_->end(); ++it ) {
        
   ctRef_  += (*it)->getReferenceTime();
  }

  return ctRef_;

}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const&  LinacCavity::getPhi()  const
{
  return phi_s_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double LinacCavity::getDesignEnergyGain()   const
{
  return strength_* cos(phi_s_); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void   LinacCavity::setFrequency( double const& freq)
{
  w_rf_ = 2.0*M_PI*freq;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double   LinacCavity::getFrequency() const
{
  return w_rf_/(2.0*M_PI);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& LinacCavity::getRadialFrequency() const
{
  return w_rf_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

 void  LinacCavity::setPhi( double const& radians)
{
  phi_s_ = radians;

  boost::dynamic_pointer_cast<LCavityUpstream>(bml_->firstElement())->setPhi(phi_s_);
  boost::dynamic_pointer_cast<LCavityDnstream>(bml_->lastElement())->setPhi(phi_s_);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void  LinacCavity::setStrength( double const& eV)
{
  strength_ = eV*1.0e-9; 

  double length1 = bml_->firstElement()->Length();
  double length2 = bml_->lastElement()->Length();

  bml_->firstElement()->setStrength(strength_*length1/length_ );
  bml_->lastElement()->setStrength(strength_ *length2/length_ );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void  LinacCavity::setLength( double const& )
{
  (*pcerr) <<   "*** WARNING ****: "
              "\n*** WARNING ****: Attempt to explicitly set the length of a LinacCavity. "
              "\n*** WARNING ****: This is most likely an error.  Will not comply."
              "\n*** WARNING ****: Continuing, nontheless... "
              "\n*** WARNING ****: " 
           << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void  LinacCavity::setWakeOn( bool set )
{
  boost::dynamic_pointer_cast<LinacCavity::Propagator>(propagator_)->setWakeOn(*this, set);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

bool LinacCavity::wakeOn() const
{
  return boost::dynamic_pointer_cast<LinacCavity::Propagator>(propagator_)->wakeOn( *this );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void LinacCavity::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  //----------------------------------------------------------------------
  // For a LinacCavity, strength_ represents the integrated strength 
  // Splitting the cavity results in different value of strength_ both 
  // each part.
  //----------------------------------------------------------------------

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double const& pc, ElmPtr& a, ElmPtr& b )", 
           uic.str().c_str() ) );
  }

  a = LinacCavityPtr( Clone() );
  b = LinacCavityPtr( Clone() );

  a->rename( Name() + string("_1") );
  b->rename( Name() + string("_2") );
  
  a->setStrength( pc*Strength() );
  a->setLength(   pc*Length()   );

  b->setStrength( (1.0 - pc)*Strength() );
  b->setLength(   (1.0 - pc)*Length()   );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void LinacCavity::localPropagate( Particle& p)
{
  (*propagator_)(*this,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void LinacCavity::localPropagate( JetParticle& p)
{
  (*propagator_)(*this,p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void LinacCavity::localPropagate( ParticleBunch& b)
{
  (*propagator_)(*this,b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void LinacCavity::localPropagate( JetParticleBunch& b)
{
  (*propagator_)(*this,b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
