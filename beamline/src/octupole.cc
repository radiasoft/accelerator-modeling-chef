/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      octupole.cc
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
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iosetup.h>
#include <octupole.h>
#include <drift.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class octupole
// **************************************************

octupole::octupole( double l, double s ) : bmlnElmnt(l, s) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( const char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
 p_bml = new beamline;
 p_bml->append( new drift( length / 2.0 ) );
 p_bml->append( p_bml_e = new thinOctupole( strength*length ) );
 p_bml->append( new drift( length / 2.0 ) );
}

octupole::octupole( const octupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

octupole::~octupole() {
  p_bml_e = 0;
}


void octupole::setStrength( double s ) {
 strength = s - getShunt()*IToField();
 p_bml_e->setStrength( strength*length );
}


void octupole::setStrength( double, int ) {
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "void octupole::setStrength( double, int ) {", 
        "Call to unwritten function." ) );
}


void octupole::setCurrent( double s ) {
 p_bml_e->setCurrent( s );
}


const char* octupole::Type() const 
{ 
  return "octupole"; 
}


bool octupole::isMagnet() const
{
  return true;
}


void octupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void octupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length.
  *a = new octupole(         pc  *length, strength );
  *b = new octupole( ( 1.0 - pc )*length, strength );

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
//   class thinOctupole
// **************************************************

thinOctupole::thinOctupole ( double s ) : bmlnElmnt( 0.0, s ) {
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
}

thinOctupole::thinOctupole ( const char* n, double s ) : bmlnElmnt( n, 0.0, s ) {
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
}


thinOctupole::thinOctupole( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinOctupole::thinOctupole( const thinOctupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinOctupole::~thinOctupole() {
}


const char* thinOctupole::Type() const 
{ 
  return "thinOctupole"; 
}


bool thinOctupole::isMagnet() const
{
  return true;
}
