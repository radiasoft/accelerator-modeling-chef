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
**************************************************************************
**************************************************************************
*************************************************************************/

#include <iomanip>
#include <beamline/LinacCavity.h>
#include <beamline/BmlVisitor.h>
#include <beamline/RefRegVisitor.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LinacCavity::LinacCavity( const char* name,         // name
                          double const& length,     // length [m]
                          double const& f,          // rf frequency 
                          double const& eV,         // rf voltage 
                          double const& phi_s)      // synchronous phase 
 : bmlnElmnt( name, length,  eV*1.0e-9), w_rf_(2*M_PI*f), phi_s_(phi_s)

{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::LinacCavity( LinacCavity const& x ) 
 : bmlnElmnt( x.ident_.c_str(), x.length_, x.strength_), w_rf_(x.w_rf_), phi_s_(x.phi_s_)
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
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
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
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  

void  LinacCavity::setStrength( double const& eV)
{

  strength_ = eV*1.0e-9; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||  
