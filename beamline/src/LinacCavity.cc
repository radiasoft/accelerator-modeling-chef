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
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>
#include <beamline/LinacCavity.h>
#include <beamline/BmlVisitor.h>
#include <beamline/RefRegVisitor.h>

using namespace std;


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


LinacCavity::LinacCavity( const char* name_arg, // name
                          double const& lng_arg,    // length [m]
                          double const& f_arg,      // rf frequency 
                          double const& eV_arg,     // rf voltage 
                          double const& phi_s_arg,  // synchronous phase 
                          double const& Q_arg,      // Quality factor 
                          double const& R_arg )     // Shunt Impedance 
 : rfcavity( name_arg, lng_arg, f_arg, eV_arg, (M_PI/2.0 + phi_s_arg*(2.0*M_PI)), Q_arg, R_arg )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

LinacCavity::LinacCavity( LinacCavity const& x ) 
:   rfcavity( x ) 
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
     << (w_rf_/MATH_TWOPI)
     << " " << phi_s_
     << " " << Q_
     << " " << R_
     << " " << h_
     << '\n';
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


istream& LinacCavity::readFrom(istream& is) 
{
  double w;
  is >> w 
     >> phi_s_ 
     >> Q_ 
     >> R_
     >> h_;
  w_rf_      = w*MATH_TWOPI;
  sin_phi_s_ = sin(phi_s_);
  rfcavity::finishConstructor();
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

void LinacCavity::acceptInner( BmlVisitor& v ) {

  rfcavity::acceptInner(v);

}  


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void LinacCavity::acceptInner( ConstBmlVisitor& v ) const 
{

  rfcavity::acceptInner(v);

}

