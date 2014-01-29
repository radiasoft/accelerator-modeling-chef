/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      thinMultipole.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws.
******                                                             
****** 
******  Author: ostiguy@fnal.gov
******                                                               
**************************************************************************
*************************************************************************/

#include <beamline/ThinMultipolePropagators.h>
#include <beamline/thinMultipole.h>
#include <beamline/BmlVisitor.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole () 
 : bmlnElmnt( "", 0.0, 0.0 ) 
{

 static bool firstCall = true;

 if ( firstCall ) {
   (*pcerr) << "\n*** SORRY:                                         ";
   (*pcerr) << "\n*** SORRY: class thinMultipole is not implemented. ";
   (*pcerr) << "\n*** SORRY: A marker will be substituted            ";
   (*pcerr) << "\n*** SORRY: for each instance.                      ";
   (*pcerr) << "\n*** SORRY:                                       \n";
   (*pcerr) << endl;   
   firstCall = false;
 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole ( char const* name) 
 : bmlnElmnt( name, 0.0, 0.0 ) 
{

 static bool firstCall = true;

 if ( firstCall ) {
   (*pcerr) << "\n*** SORRY:                                         ";
   (*pcerr) << "\n*** SORRY: class thinMultipole is not implemented. ";
   (*pcerr) << "\n*** SORRY: A marker will be substituted            ";
   (*pcerr) << "\n*** SORRY: for each instance.                      ";
   (*pcerr) << "\n*** SORRY:                                       \n";
   (*pcerr) << endl;   
   firstCall = false;
 }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


thinMultipole::thinMultipole ( const char* n, double const& s ) 
: bmlnElmnt( n, 0.0, s ) 
{
 static bool firstCall = true;

 if ( firstCall ) {
   (*pcerr) << "\n*** SORRY:                                         ";
   (*pcerr) << "\n*** SORRY: class thinMultipole is not implemented. ";
   (*pcerr) << "\n*** SORRY: A marker will be substituted            ";
   (*pcerr) << "\n*** SORRY: for each instance.   Length and tilt    ";
   (*pcerr) << "\n*** SORRY: information are being ignored.          ";
   (*pcerr) << "\n*** SORRY:                                       \n";
   (*pcerr) << endl;   
   firstCall = false;
 }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::thinMultipole(  thinMultipole const& x ) 
: bmlnElmnt( x )
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinMultipole::~thinMultipole() 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinMultipole::Type() const 
{ 
  return "thinMultipole"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinMultipole::isMagnet() const
{
  return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::accept( BmlVisitor& v ) 
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::accept( ConstBmlVisitor& v ) const
{
  v.visit(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::localPropagate( Particle& p )
{
  (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::localPropagate( JetParticle& p )
{
  (*propagator_)(*this, p);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::localPropagate( ParticleBunch& b )
{
  (*propagator_)(*this, b);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinMultipole::localPropagate( JetParticleBunch& b )
{
  (*propagator_)(*this, b);
}

