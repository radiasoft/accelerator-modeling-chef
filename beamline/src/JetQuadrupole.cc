/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      JetQuadrupole.cc
******                                                                
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "JetQuadrupole.h"


using namespace std;

// **************************************************
//   class JetQuadrupole
// **************************************************

JetQuadrupole::JetQuadrupole()
: bmlnElmnt()
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "JetQuadrupole::JetQuadrupole()", 
         "Cannot define JetQuadrupole without parameters." ) );
}

JetQuadrupole::JetQuadrupole( char* n, double l, double s, int index, int m,
                              PropFunc* pf ) 
: bmlnElmnt( n, l, s, pf )
{
 if( l<= 0.0 ) {
   ostringstream uic;
   uic  << "Argument l, the length, is negative: l = " << l;
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "JetQuadrupole::JetQuadrupole( char* n, double l, double s, int index, int m,", 
          uic.str().c_str() ) );
 }

 JetStrength.setVariable(index);
 strengthIndex = index;

 if( 0 == strcmp( pf->Type(), "JetQuadrupole::TPOT_Prop" ) ) 
 {
   ((JetQuadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }

}


JetQuadrupole::JetQuadrupole( double l, double s, int index, int m,
                              PropFunc* pf ) 
: bmlnElmnt( l, s, pf ) 
{
 // This body is identical to that of the previous constructor.

 if( l<= 0.0 ) {
  ostringstream uic;
  uic  << "Argument l, the length, is negative: l = " << l;
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "JetQuadrupole::JetQuadrupole( double l, double s, int index, int m, PropFunc* pf ) ",
         uic.str().c_str() ) );
 }

 JetStrength.setVariable(index);
 strengthIndex = index;

 if( 0 == strcmp( pf->Type(), "JetQuadrupole::TPOT_Prop" ) ) 
 {
   ((JetQuadrupole::TPOT_Prop*) pf)->setup( this );
 }
 else 
 {
   p_bml = 0;
 }
}


JetQuadrupole::JetQuadrupole( bmlnElmntData& x ) 
: bmlnElmnt( x ) 
{
   throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
          "JetQuadrupole::JetQuadrupole( bmlnElmntData& x ) ", 
          "Not allowed." ) );
}


JetQuadrupole::JetQuadrupole( const JetQuadrupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  JetStrength = x.JetStrength;
  strengthIndex = x.strengthIndex;
//  cout << "Copy constructor " << JetStrength << endl;
}


JetQuadrupole::~JetQuadrupole() {
}


void JetQuadrupole::setStrength( double s ) {
 JetStrength.setVariable(strengthIndex);
// JetStrength += s;
 if(p_bml_e != 0)
 p_bml_e->setStrength( strength*length, strengthIndex );
}


void JetQuadrupole::setStrength( double s, int index) {
 strength = s;
 strengthIndex = index;
 JetStrength.setVariable(s,index);
// JetStrength += s;
 if(p_bml_e != 0)
 p_bml_e->setStrength( strength*length, index );
}


void JetQuadrupole::eliminate() {
 delete this;
}



const char* JetQuadrupole::Type() const 
{ 
  return "JetQuadrupole"; 
}


void JetQuadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void JetQuadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  *a = new JetQuadrupole( ident,        pc  *length, strength,strengthIndex, 
                       4, Propagator ); // ??? Fix
  *b = new JetQuadrupole( ident,( 1.0 - pc )*length, strength, strengthIndex,
                       4, Propagator ); // ??? this
  // We assume "strength" means field, not field*length.
}

ostream& JetQuadrupole::writeTo(ostream& os)
{
  os << strengthIndex << " ";
  if ( Propagator == &JetQuadrupole::LikeTPOT )
    os << "JetQuadrupole::P_LikeTPOT  JetQuadrupole::J_LikeTPOT";
  else 
    os << "UNKNOWN   UNKNOWN";

  os << "\n";
  return os;
}

istream& JetQuadrupole::readFrom(istream& is)
{
  int index;
  char prop_fun[100], jet_prop_fun[100];
  
  is >> index >> prop_fun >> jet_prop_fun;
  setStrength(strength, index);

  if ( strcasecmp( prop_fun, "JetQuadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction( &JetQuadrupole::LikeTPOT );
  else
    {
      cerr << " **** WARNING **** JetQuadrupole::readFrom(istream)\n";
      cerr << " **** WARNING **** unknown propagator function specified:\n";
      cerr << " **** WARNING **** " << prop_fun << "\n";
      cerr << " **** WARNING **** Substituting JetQuadrupole::P_LikeTPOT\n";
      setPropFunction( &JetQuadrupole::LikeTPOT );
    }

  return is;
}


// **************************************************
//   class JetthinQuad
// **************************************************

JetthinQuad::JetthinQuad( double s, int index ) 
 : bmlnElmnt() 
{
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

JetthinQuad::JetthinQuad( char*  n, 
                          double s,
                          int    index ) 
 : bmlnElmnt(n) 
{
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}

JetthinQuad::JetthinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "JetthinQuad::JetthinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {", 
        "Not allowed." ) );
}


JetthinQuad::JetthinQuad( const JetthinQuad& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  KL = x.KL;
  strengthIndex = x.strengthIndex;
}

JetthinQuad::~JetthinQuad() {
}

void JetthinQuad::setStrength( double s ) {
 KL.setVariable( s, strengthIndex );
 strength = s;
}


void JetthinQuad::setStrength( double s, int index ) {
 KL.setVariable( s, index );
 strength = s;
 strengthIndex = index;
}


void JetthinQuad::eliminate() {
 delete this;
}


const char* JetthinQuad::Type() const 
{ 
  return "JetthinQuad"; 
}


ostream& JetthinQuad::writeTo(ostream& os)
{
  os << strengthIndex << "\n";
  return os;
}

istream& JetthinQuad::readFrom(istream& is)
{
  is >> strengthIndex;
  return is;
}

