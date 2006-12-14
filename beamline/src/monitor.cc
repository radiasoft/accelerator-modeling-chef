/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      monitor.cc
******  Version:   3.0
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/monitor.h>
#include <beamline/Particle.h>


using namespace std;


// **************************************************
//   class monitor 
// **************************************************

monitor::monitor() 
: bmlnElmnt(),
  _outputStreamPtr( &std::cout ), 
  _onOffSwitch( false ), 
  _driftFraction( 0.5 )
{


  _rgr   = new double [ BMLN_dynDim ];
  for( int i = 0; i < BMLN_dynDim; i++ ) { _rgr[i] = 0.0; }

}


monitor::monitor( const char* n ) 
: bmlnElmnt(n),
  _outputStreamPtr( &std::cout ), 
  _onOffSwitch( false ), 
  _driftFraction( 0.5 )
{

  _rgr   = new double [ BMLN_dynDim ];
  for( int i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = 0.0;
}


monitor::monitor( const char* n, double const& l )
: bmlnElmnt(n,l),
  _outputStreamPtr( &std::cout ), 
  _onOffSwitch( false ), 
  _driftFraction( 0.5 )
{
  _rgr   = new double [ BMLN_dynDim ];
  for( int i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = 0.0;
}


monitor::monitor( monitor const& x )
: bmlnElmnt( x ),
  _outputStreamPtr( x._outputStreamPtr ), 
  _onOffSwitch( x._onOffSwitch ), 
  _driftFraction( x._driftFraction )
{

  _rgr   = new double [ BMLN_dynDim ];
  for( int i = 0; i < BMLN_dynDim; i++ ) _rgr[i] = x._rgr[i];
}


monitor::~monitor() 
{
 delete [] _rgr;
}


void monitor::setOutputStream( ostream& x )
{
  if( x.good() ) { _outputStreamPtr = &x; }
  else {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void monitor::setOutputStream( ostream& x )", 
           "Argument x is not available for writing." ) );
  }
}


void monitor::setOutputStream( ostream* x )
{
  this->setOutputStream( *x );
}


bool monitor::on() 
{
 static bool ret;
 ret = _onOffSwitch;
 _onOffSwitch = true;
 return ret;
}


bool monitor::off() 
{
 static bool ret;
 ret = _onOffSwitch;
 _onOffSwitch = false;
 return ret;
}


const char* monitor::version()
{
  return "3.0";
}


const char* monitor::Type() const 
{ 
  return "monitor"; 
}


double monitor::operator[]( int n ) 
{
 if( n < 0 || n > 5 ) return 0.0;
 return _rgr[n];
}

ostream& monitor::writeTo(ostream &os) 
{
  if ( Length() != 0 ) 
    os << _driftFraction << endl;
  return os;
}

istream& monitor::readFrom(istream &is) 
{
  if ( Length() != 0 ) {
    is >> _driftFraction ;
    if ( _driftFraction < 0 || _driftFraction > 1 ) {
      cerr << "monitor::readFrom(istream&): Read a drift fraction of " 
           << _driftFraction << "; substituting 0.5" 
           << endl;
      _driftFraction = 0.5;
    }
  } else {
    _driftFraction = 0.5; // Basically irrelevant if the monitor has no length!
  }
  return is;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class vmonitor 
// **************************************************

vmonitor::vmonitor() : monitor() 
{
}

vmonitor::vmonitor( const char* n ) : monitor( n )
{
}

vmonitor::vmonitor( const char* n, double const& l ) : monitor( n,l )
{
}

vmonitor::vmonitor( vmonitor const& x ) 
: monitor( x ) 
{

}

vmonitor::~vmonitor() 
{
}

double vmonitor::operator[]( int n ) 
{
 if( n == 0 ) return _rgr[1];
 if( n == 1 ) return _rgr[4];
 return 0.0;
}


const char* vmonitor::Type() const 
{ 
  return "vmonitor"; 
}


// **************************************************
//   class hmonitor 
// **************************************************

hmonitor::hmonitor() : monitor() 
{
}

hmonitor::hmonitor( const char* n ) : monitor( n )
{
}

hmonitor::hmonitor( const char* n, double const& l ) : monitor( n, l )
{
}

hmonitor::hmonitor( const hmonitor& x ) 
: monitor( x ) 
{}



hmonitor::~hmonitor() 
{}

double hmonitor::operator[]( int n ) 
{
 if( n == 0 ) return _rgr[0];
 if( n == 1 ) return _rgr[3];
 return 0.0;
}


const char* hmonitor::Type() const 
{ 
  return "hmonitor"; 
}

