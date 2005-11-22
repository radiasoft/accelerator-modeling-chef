/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      sextupole.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
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
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iosetup.h>
#include <sextupole.h>
#include <drift.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class sextupole 
// **************************************************

sextupole::sextupole ( double l, double s ) : bmlnElmnt( l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}


sextupole::sextupole ( const char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}


sextupole::sextupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

sextupole::sextupole( const sextupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{

}

sextupole::~sextupole() {
  p_bml_e = 0;
}


void sextupole::setStrength( double s ) {
  strength = s - getShunt()*IToField();
  p_bml_e->setStrength( strength*length );
}


void sextupole::setStrength( double, int ) {
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "void sextupole::setStrength( double, int ) {", 
         "Function should not be called." ) );
}


void sextupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


const char* sextupole::Type() const 
{ 
  return "sextupole"; 
}


void sextupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sextupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length.
  *a = new sextupole(         pc  *length, strength );
  *b = new sextupole( ( 1.0 - pc )*length, strength );

  // Rename
  char* newname;
  newname = new char [ strlen(ident) + 6 ];

  strcpy( newname, ident );
  strcat( newname, "_1" );
  (*a)->Rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_2" );
  (*b)->Rename( newname );

  delete [] newname;
}



// **************************************************
//   class thinSextupole
// **************************************************

thinSextupole::thinSextupole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
}

thinSextupole::thinSextupole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
}


thinSextupole::thinSextupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinSextupole::thinSextupole( const thinSextupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinSextupole::~thinSextupole() {
}


const char* thinSextupole::Type() const { 
  return "thinSextupole"; 
}
