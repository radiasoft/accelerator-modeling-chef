/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      sextupole.cc
******  Version:   2.1
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

#include <beamline/beamline.h>
#include <beamline/sextupole.h>
#include <beamline/drift.h>

using namespace std;


// **************************************************
//   class sextupole 
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole () : bmlnElmnt( 1.0, 0.0 ) {
 p_bml = new beamline;
 p_bml->append( new drift( 1.0 / 2.0 ) );
 p_bml->append( p_bml_e =  new thinSextupole( 0.0 ) );
 p_bml->append( new drift( 1.0 / 2.0 ) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole ( double l, double s ) : bmlnElmnt( l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( new thinSextupole( strength*length ) ); 
 p_bml->append( new drift( length / 2.0 ) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sextupole::sextupole ( const char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinSextupole( strength*length ) ); 
 p_bml->append( new drift( length / 2.0 ) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sextupole::sextupole( sextupole const& x ) 
: bmlnElmnt( x ){
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::~sextupole() {
  // destruction completely taken care of by ~bmlnElmnt() 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setStrength( double s ) {
  strength = s - getShunt()*IToField();
  p_bml_e->setStrength( strength*length );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* sextupole::Type() const 
{ 
  return "sextupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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
  (*a)->rename( newname );

  strcpy( newname, ident );
  strcat( newname, "_2" );
  (*b)->rename( newname );

  delete [] newname;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sextupole::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class thinSextupole
// **************************************************

thinSextupole::thinSextupole () : bmlnElmnt( 0.0, 0.0 ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::thinSextupole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::thinSextupole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/2)*B''l in  Tesla / meter
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


thinSextupole::thinSextupole( thinSextupole const& x ) 
: bmlnElmnt( x ){}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinSextupole::~thinSextupole() {}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinSextupole::Type() const { 
  return "thinSextupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinSextupole::isMagnet() const
{
  return true;
}


