/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Monitor.cc
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
****** Feb 2009           ostiguy@fnal.gov
****** - refactored with new hierarchy (MonitorBase). 
******   so that - for example - HMonitor has no public 
*******  method to read vertical position.      
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator                                                              
****** - explicit implementation for assignment operator                                                               
****** Dec 2007           ostiguy@fnal.gov
****** -new typesafe propagators
****** -support for misaligments and errors 
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
******
**************************************************************************
*************************************************************************/

#include <beamline/Monitor.h>
#include <beamline/MonitorPropagators.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>
#include <boost/random.hpp>
#include <boost/bind.hpp>


using namespace std;

//-------------------------------------------------
// gaussian random number generator for BPM errors.
//-------------------------------------------------

namespace { 
  
  typedef boost::mt19937       base_generator_type; 
  base_generator_type          rng;                    // pseudo-random number generator
  boost::normal_distribution<> gauss(0.0,1.0 );        
  boost::variate_generator<base_generator_type&, boost::normal_distribution<> > bpm_error(rng, gauss);             

  double ( boost::variate_generator<base_generator_type&, boost::normal_distribution<> >::* mfptr )() = 
   &boost::variate_generator<base_generator_type&, boost::normal_distribution<> >::operator();
}  

boost::function<double()> MonitorBase::nrnd_ = boost::bind( mfptr, &bpm_error );

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase::MonitorBase() 
: BmlnElmnt(),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
            xrerr_(0.0),
            yrerr_(0.0)
{}  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase::MonitorBase( std::string const& n ) 
: BmlnElmnt(n),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
            xrerr_(0.0),
            yrerr_(0.0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase::MonitorBase( std::string const& n, double const& l )
: BmlnElmnt(n,l),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
            xrerr_(0.0),
	    yrerr_(0.0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase::MonitorBase( MonitorBase const& o )
: BmlnElmnt( o ),
          enabled_( o.enabled_         ), 
    driftFraction_( o.driftFraction_   ),
               dx_( o.dx_              ),  
               dy_( o.dy_              ),
              npx_( o.npx_             ),  
              npy_( o.npy_             ),
            xrerr_( o.xrerr_           ),
            yrerr_( o.yrerr_           )   
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase::~MonitorBase() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

MonitorBase& MonitorBase::operator=( MonitorBase const& rhs)
{
  if (this == &rhs) return *this;
  BmlnElmnt::operator=(rhs);
  enabled_       = rhs.enabled_;       
  driftFraction_ = rhs.driftFraction_; 
  dx_            = rhs.dx_;             
  dy_            = rhs.dy_;            
  npx_           = rhs.npx_;            
  npy_           = rhs.npy_;           
  xrerr_         = rhs.xrerr_;         
  yrerr_         = rhs.yrerr_;         
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::enable( bool set) 
{
  enabled_= set;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::isEnabled() const 
{
 return enabled_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::isMagnet() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::isThin() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::isPassive() const 
{ 
  return true; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool MonitorBase::isDriftSpace() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& MonitorBase::writeTo(ostream &os) 
{
  if ( Length() != 0 ) 
    os << driftFraction_ << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& MonitorBase::readFrom(istream &is) 
{
  if ( Length() != 0 ) {
    is >> driftFraction_ ;
    if (  driftFraction_ < 0 || driftFraction_ > 1 ) {
      cerr << "monitor::readFrom(istream&): Read a drift fraction of " 
           << driftFraction_ << "; substituting 0.5" 
           << endl;
      driftFraction_ = 0.5;
    }
  } else {
     driftFraction_ = 0.5; // Basically irrelevant if the monitor has no length!
  }
  return is;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MonitorBase::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void MonitorBase::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double MonitorBase::setDriftFraction( double f ) 
{ 
  double ret = driftFraction_; 
  if ( f <= 1 && f >= 0 ) driftFraction_ = f; 
  return ret; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& MonitorBase::getDriftFraction() const
{ 
  return driftFraction_; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::HMonitor() 
 : MonitorBase() 
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::HMonitor( std::string const& n ) 
: MonitorBase( n )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::HMonitor( std::string const& n, double const& l ) 
: MonitorBase( n, l )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::HMonitor( HMonitor const& o ) 
: MonitorBase( o )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor::~HMonitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

HMonitor& HMonitor::operator=( HMonitor const& rhs)
{
  if (this == &rhs) return *this;
  MonitorBase::operator=(rhs);
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double HMonitor::hposition() const
{ 
  return  (enabled_) ?  ( xpos_ + xrerr_* nrnd_()  + dx_  ) :  ( xrerr_* nrnd_() );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::setHposition( double const& value ) const
{ 
  xpos_  = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  HMonitor::npx() const
{
  return npx_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void HMonitor::setNpx(double const& value  ) const
{
  npx_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* HMonitor::Type() const 
{ 
  return "HMonitor"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::VMonitor() 
: MonitorBase() 
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::VMonitor( std::string const& n ) 
: MonitorBase( n )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::VMonitor( std::string const& n, double const& l ) 
: MonitorBase( n,l )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::VMonitor( VMonitor const& o ) 
: MonitorBase( o )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor::~VMonitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

VMonitor& VMonitor::operator=( VMonitor const& rhs)
{
  if (this == &rhs) return *this;
  MonitorBase::operator=(rhs);
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double VMonitor::vposition() const
{ 
  return  (enabled_) ?  ( ypos_ + yrerr_* nrnd_()  + dy_  ) :  ( yrerr_* nrnd_() );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::setVposition( double const& value ) const
{ 
  ypos_  = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  VMonitor::npy() const
{
  return npy_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void VMonitor::setNpy(double const& value  ) const
{
  npy_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* VMonitor::Type() const 
{ 
  return "VMonitor"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Monitor() 
  : MonitorBase()
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Monitor( std::string const& n ) 
: MonitorBase( n )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Monitor( std::string const& n, double const& l )
  : MonitorBase( n,l )
{
  propagator_ = PropagatorPtr(new Propagator(*this) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::Monitor( Monitor const& o )
: MonitorBase( o)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Monitor::~Monitor()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Monitor::hposition() const
{ 
  return  (enabled_) ?  ( xpos_ + xrerr_* nrnd_()  + dx_  ) :  ( xrerr_* nrnd_() );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double Monitor::vposition() const
{ 
  return  (enabled_) ?  ( ypos_ + yrerr_* nrnd_()  + dy_  ) :  ( yrerr_* nrnd_() );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::setHposition( double const& value ) const
{ 
  xpos_  = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::setVposition( double const& value ) const
{ 
  ypos_  = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  Monitor::npx() const
{
  return npx_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  Monitor::npy() const
{
  return npy_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::setNpx(double const& value  ) const
{
  npx_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Monitor::setNpy(double const& value  ) const
{
  npy_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* Monitor::Type() const 
{ 
  return "Monitor"; 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
