/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitor.cc
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
****** Dec 2007           ostiguy@fnal.gov
****** -new typesafe propagators
****** -support for misaligments and errors 
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator                                                              
****** - explicit implementation for assignment operator                                                               
**************************************************************************
*************************************************************************/

#include <beamline/monitor.h>
#include <beamline/MonitorPropagators.h>
#include <beamline/Particle.h>
#include <beamline/BmlVisitor.h>
#include <boost/random.hpp>


using namespace std;

//-------------------------------------------------
// gaussian random number generator for BPM errors.
//-------------------------------------------------

namespace { 
  
  boost::mt19937               rng;              // pseudo-random number generator
  boost::normal_distribution<> gauss(0.0,1.0 );        
  boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > error(rng, gauss);             
}  

// **************************************************
//   class monitor 
// **************************************************

monitor::monitor() 
: bmlnElmnt(),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
             xrerr_(0.0),
             yrerr_(0.0)
             
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}  

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

monitor::monitor( std::string const& n ) 
: bmlnElmnt(n),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
             xrerr_(0.0),
             yrerr_(0.0)
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

monitor::monitor( std::string const& n, double const& l )
: bmlnElmnt(n,l),
          enabled_( true ), 
    driftFraction_( 0.5 ),
               dx_(0.0),  
               dy_(0.0),
              npx_(0.0),  
              npy_(0.0),
            xrerr_(0.0),
            yrerr_(0.0)
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

monitor::monitor( monitor const& o )
: bmlnElmnt( o ),
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

monitor::~monitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

monitor& monitor::operator=( monitor const& rhs)
{
  if (this == &rhs) return *this;
  bmlnElmnt::operator=(rhs);
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

bool monitor::enable( bool set) 
{
  enabled_= set;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool monitor::isEnabled() const 
{
 return enabled_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* monitor::Type() const 
{ 
  return "monitor"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool monitor::isMagnet() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool monitor::isThin() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool monitor::isPassive() const 
{ 
  return true; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool monitor::isDriftSpace() const 
{ 
  return false; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& monitor::writeTo(ostream &os) 
{
  if ( Length() != 0 ) 
    os << driftFraction_ << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

istream& monitor::readFrom(istream &is) 
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

void monitor::accept( BmlVisitor& v )            
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double monitor::setDriftFraction( double f ) 
{ 
  double ret = driftFraction_; 
  if ( f <= 1 && f >= 0 ) driftFraction_ = f; 
  return ret; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& monitor::getDriftFraction() const
{ 
  return driftFraction_; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double monitor::hposition() const
{ 
  return (enabled_) ?   (xpos_  +  xrerr_* ::error()  + dx_ ) : ( xrerr_* ::error()); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double monitor::vposition() const
{ 
  return  (enabled_) ?  ( ypos_ + yrerr_* ::error()  + dy_  ) :  ( yrerr_* ::error() );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::setHposition( double const& value ) const
{ 
  xpos_  = value;
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::setVposition( double const& value ) const
{ 
  ypos_  = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& monitor::npx() const
{
  return npx_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double const& monitor::npy() const
{
  return npy_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::setNpx( double const& value ) const
{
  npx_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void monitor::setNpy(double const& value  ) const
{
  npy_ = value;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class vmonitor 
// **************************************************

vmonitor::vmonitor() 
: monitor() 
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vmonitor::vmonitor( std::string const& n ) 
: monitor( n )
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vmonitor::vmonitor( std::string const& n, double const& l ) 
: monitor( n,l )
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vmonitor::vmonitor( vmonitor const& x ) 
: monitor( x )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vmonitor::~vmonitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

vmonitor& vmonitor::operator=( vmonitor const& rhs)
{
  if (this == &rhs) return *this;
  monitor::operator=(rhs);
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* vmonitor::Type() const 
{ 
  return "vmonitor"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class hmonitor 
// **************************************************

hmonitor::hmonitor() : monitor() 
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hmonitor::hmonitor( std::string const& n ) 
: monitor( n )
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hmonitor::hmonitor( std::string const& n, double const& l ) 
: monitor( n, l )
{
  propagator_ = PropagatorPtr(new Propagator() ); 
  propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hmonitor::hmonitor( hmonitor const& x ) 
: monitor( x )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hmonitor::~hmonitor() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

hmonitor& hmonitor::operator=( hmonitor const& rhs)
{
  if (this == &rhs) return *this;
  monitor::operator=(rhs);
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* hmonitor::Type() const 
{ 
  return "hmonitor"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
