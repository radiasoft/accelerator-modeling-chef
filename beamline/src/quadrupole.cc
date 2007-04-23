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
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <beamline/quadrupole.h>
#include <beamline/drift.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;



// **************************************************
//   class quadrupole
// **************************************************

quadrupole::quadrupole()
: bmlnElmnt( 1.0, 0.0, &quadrupole::LikeTPOT )
{
  setupPropFunc();
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::quadrupole( const char* n, double const& l, double const& s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( n, l, s, pf )
{
 if( l<= 0.0 ) {
  ostringstream uic;
  uic  << "Quadrupole length l = " << l << " is negative.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "quadrupole::quadrupole( const char* n, double const& l, double const& s, bmlnElmnt::PropFunc* pf )", 
         uic.str().c_str() ) );
 }

 setupPropFunc();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::quadrupole( double const& l, double const& s, bmlnElmnt::PropFunc* pf )
: bmlnElmnt( l, s, pf )
{
 if( l<= 0.0 ) {
  ostringstream uic;
  uic  << "Quadrupole length l = " << l << " is negative.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "quadrupole::quadrupole( double const& l, double const& s, bmlnElmnt::PropFunc* pf )", 
         uic.str().c_str() ) );
 }

  setupPropFunc();

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::quadrupole( quadrupole const& x ) 
: bmlnElmnt( x )
{
  setupPropFunc();

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

quadrupole::~quadrupole() 
{
  releasePropFunc();
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::setStrength( double const& s ) {


  strength_                  = s - getShunt()*IToField();

  double integratedStrength = strength_*length_;

  if( p_bml_) 
  {
   int counter = 0;

   for ( beamline::iterator it  = p_bml_->begin();
	                    it != p_bml_->end(); ++it ) {
     if( typeid(**it) == typeid(thinQuad ) )  ++counter;
   }


   if( counter <= 0 ) {
     throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void quadrupole::setStrength( double const& s ) {", 
            "No thin quads in the internal beamline." ) );
   }
   else if( counter == 1) {
     if(bml_e_) 
     {
       bml_e_->setStrength( integratedStrength );
     }
     else 
     {
     throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
            "void quadrupole::setStrength( double const& s ) {", 
            "bml_e_ not set." ) );
     }
   }
   else {

       for ( beamline::iterator it  = p_bml_->begin(); it != p_bml_->end(); ++it ) {
       if( typeid(**it) == typeid(thinQuad) ) {
           (*it)->setStrength( integratedStrength/counter );
       }
     }
   }
 }

}

///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::releasePropFunc()
{
  if( !propfunc_ ) return;

  { bml_e_ = ElmPtr(); p_bml_ = BmlPtr();  propfunc_ = 0; }

}

///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
///|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void quadrupole::setupPropFunc()
{
  if( !propfunc_ ) {
    (*pcerr) << "\n**** WARNING **** "
            "\n**** WARNING **** void quadrupole::setupPropFunc()"
            "\n**** WARNING **** Invoked with a null propagator functor."
            "\n**** WARNING **** No action taken, but your program will"
            "\n**** WARNING **** probably crash soon."
            "\n**** WARNING **** "
         << endl;

 }

   if( 0 == strcmp( propfunc_->Type(), "quadrupole::TPOT_Prop" ) ) {

      ((quadrupole::TPOT_Prop*) propfunc_)->setup( this );
   
      // This method will create p_bml_
   };

}



//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* quadrupole::Type() const 
{ 
  return "quadrupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool quadrupole::isMagnet() const
{
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void quadrupole::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.

  a = QuadrupolePtr( new quadrupole(         pc  *length_, strength_, propfunc_ ) ); // ??? Fix
  b = QuadrupolePtr( new quadrupole( ( 1.0 - pc )*length_, strength_, propfunc_ ) ); // ??? this

  // Rename


  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& quadrupole::writeTo(ostream& os)
{
  if ( propfunc_ == 		&quadrupole::LikeMAD )
    os << "quadrupole::P_LikeMAD   quadrupole::J_LikeMAD";
  else if ( propfunc_ == 	&quadrupole::LikeTPOT )
    os << "quadrupole::P_LikeTPOT  quadrupole::J_LikeTPOT";
  else 
    os << "UNKNOWN  UNKNOWN";

  os << "\n";
  return os;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::accept( BmlVisitor& v )            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void quadrupole::accept( ConstBmlVisitor& v ) const { 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class thinQuad
// **************************************************

thinQuad::thinQuad() : bmlnElmnt() {
 strength_ = 0.0;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::thinQuad( double const& s ) : bmlnElmnt() {
 strength_ = s;      // B'L in Tesla
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::thinQuad( const char* n, double const& s ) : bmlnElmnt(n) {
 strength_ = s;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinQuad::thinQuad( const thinQuad& x ) 
: bmlnElmnt( x ){}

thinQuad::~thinQuad() {
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinQuad::Type() const { 
  return "thinQuad"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinQuad::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::accept( BmlVisitor& v )            
{  
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinQuad::accept( ConstBmlVisitor& v ) const { 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

