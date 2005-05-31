/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      quadrupole.cc
******  Version:   2.1
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

#include <iosetup.h>
#include <quadrupole.h>
#include <drift.h>
#include <BeamlineIterator.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



// **************************************************
//   class quadrupole
// **************************************************

quadrupole::quadrupole()
: bmlnElmnt()
{
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "quadrupole::quadrupole()", 
        "No default construction: Cannot define quadrupole without parameters." ) );
}

quadrupole::quadrupole( char* n, double l, double s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( n, l, s, pf )
{
 if( l<= 0.0 ) {
  ostringstream uic;
  uic  << "Quadrupole length l = " << l << " is negative.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "quadrupole::quadrupole( char* n, double l, double s, bmlnElmnt::PropFunc* pf )", 
         uic.str().c_str() ) );
 }

 this->setupPropFunc();

 // REMOVE: if( 0 == strcmp( pf->Type(), "quadrupole::TPOT_Prop" ) ) 
 // REMOVE: {
 // REMOVE:   ((quadrupole::TPOT_Prop*) pf)->setup( this );
 // REMOVE: }
 // REMOVE: else 
 // REMOVE: {
 // REMOVE:   p_bml = 0;
 // REMOVE: }
}


quadrupole::quadrupole( double l, double s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( l, s, pf )
{
 if( l<= 0.0 ) {
  ostringstream uic;
  uic  << "Quadrupole length l = " << l << " is negative.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "quadrupole::quadrupole( double l, double s, bmlnElmnt::PropFunc* pf )", 
         uic.str().c_str() ) );
 }

 this->setupPropFunc();

 // REMOVE: if( 0 == strcmp( pf->Type(), "quadrupole::TPOT_Prop" ) ) 
 // REMOVE: {
 // REMOVE:   ((quadrupole::TPOT_Prop*) pf)->setup( this );
 // REMOVE: }
 // REMOVE: else 
 // REMOVE: {
 // REMOVE:   p_bml = 0;
 // REMOVE: }
}


quadrupole::quadrupole( bmlnElmntData& x ) 
: bmlnElmnt( x ) 
{
 // ??? GET RID OF THIS FUNCTION.
  static char firstTime = 1;
  if( firstTime ) {
    (*pcerr) << "Warning: call to quadrupole::quadrupole( bmlnElmntData& x )" << endl;
    firstTime = 0;
  }

 double xm = 3.0;
 p_bml = new beamline;

 p_bml->append( new drift( length / ( 2.0*xm ) ) );
 p_bml->append( new thinQuad( strength*length/ xm ) );
 for( int i = 0; i < 2; i++ ) {
   p_bml->append( new drift( length / xm ) );
   p_bml->append( new thinQuad( strength*length/ xm ) );
 }
 p_bml->append( new drift( length / ( 2.0*xm ) ) );

}


quadrupole::quadrupole( const quadrupole& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
  this->setupPropFunc();
}


quadrupole::~quadrupole() 
{
  this->releasePropFunc();
}


void quadrupole::setStrength( double s ) {
 strength = s - getShunt()*IToField();
 double integratedStrength = strength*length;

 if( p_bml != 0 ) 
 {
   double counter = 0.0;
   BeamlineIterator bi( p_bml );
   bmlnElmnt* q;
   while( 0 != ( q = bi++  ) ) {
     if( 0 == strcmp( q->Type(), "thinQuad" ) )  counter++;
   }
   if( counter <= 0.0 ) {
     throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void quadrupole::setStrength( double s ) {", 
            "No thin quads in the internal beamline." ) );
   }
   else if( counter == 1.0 ) {
     if(p_bml_e != 0) 
     {
       p_bml_e->setStrength( integratedStrength );
     }
     else 
     {
     throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void quadrupole::setStrength( double s ) {", 
            "p_bml_e not set." ) );
     }
   }
   else {
     bi.reset();
     while( 0 != ( q = bi++  ) ) {
       if( 0 == strcmp( q->Type(), "thinQuad" ) ) {
         q->setStrength( integratedStrength/counter );
       }
     }
   }
 }
}


void quadrupole::setStrength( double, int ) {
 throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
        "void quadrupole::setStrength( double, int ) {", 
        "Not allowed." ) );
}



void quadrupole::releasePropFunc()
{
  if( 0 != Propagator ) {
    if( 0 != p_bml ) {
      delete p_bml;
      p_bml = 0;
    }
  }
  Propagator = 0;
}



void quadrupole::setupPropFunc()
{
  if( 0 != Propagator ) {
    if( 0 != p_bml ) {
      delete p_bml;
      p_bml = 0;
    }
    if( 0 == strcmp( Propagator->Type(), "quadrupole::TPOT_Prop" ) ) 
    {
      ((quadrupole::TPOT_Prop*) Propagator)->setup( this );
      // This method will create *p_bml
    }
  }
  else {
    (*pcerr) << "\n**** WARNING **** "
            "\n**** WARNING **** void quadrupole::setupPropFunc()"
            "\n**** WARNING **** Invoked with a null propagator functor."
            "\n**** WARNING **** No action taken, but your program will"
            "\n**** WARNING **** probably crash soon."
            "\n**** WARNING **** "
         << endl;
  }
}


void quadrupole::eliminate() {
 delete this;
}


const char* quadrupole::Type() const 
{ 
  return "quadrupole"; 
}


void quadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void quadrupole::Split( double pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length.
  *a = new quadrupole(         pc  *length, strength, Propagator ); // ??? Fix
  *b = new quadrupole( ( 1.0 - pc )*length, strength, Propagator ); // ??? this

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

istream& quadrupole::readFrom(istream& is)
{
  char prop_fun[100], jet_prop_fun[100];

  is >> prop_fun >> jet_prop_fun;
  if ( strcasecmp(prop_fun,             "quadrupole::P_LikeMAD" ) == 0 )
    setPropFunction(&quadrupole::LikeMAD );
  else if ( strcasecmp(prop_fun,        "quadrupole::P_LikeTPOT" ) == 0 )
    setPropFunction(&quadrupole::LikeTPOT );
  else
    {
      (*pcerr) << " **** WARNING **** quadrupole::readFrom(istream)\n";
      (*pcerr) << " **** WARNING **** unknown propagator function specified:\n";
      (*pcerr) << " **** WARNING **** " << prop_fun << "\n";
      (*pcerr) << " **** WARNING **** Substituting quadrupole::P_LikeMAD\n";
      setPropFunction(&quadrupole::LikeMAD);
    }

  return is;
}

ostream& quadrupole::writeTo(ostream& os)
{
  if ( Propagator == 		&quadrupole::LikeMAD )
    os << "quadrupole::P_LikeMAD   quadrupole::J_LikeMAD";
  else if ( Propagator == 	&quadrupole::LikeTPOT )
    os << "quadrupole::P_LikeTPOT  quadrupole::J_LikeTPOT";
  else 
    os << "UNKNOWN  UNKNOWN";

  os << "\n";
  return os;
}



// **************************************************
//   class thinQuad
// **************************************************

thinQuad::thinQuad( double s ) : bmlnElmnt() {
 strength = s;      // B'L in Tesla
}

thinQuad::thinQuad( char* n, double s ) : bmlnElmnt(n) {
 strength = s;
}

thinQuad::thinQuad( bmlnElmntData& x ) : bmlnElmnt( x ) {
}

thinQuad::thinQuad( const thinQuad& x ) 
: bmlnElmnt( (bmlnElmnt&) x )
{
}

thinQuad::~thinQuad() {
}

void thinQuad::eliminate() {
 delete this;
}


const char* thinQuad::Type() const { 
  return "thinQuad"; 
}
