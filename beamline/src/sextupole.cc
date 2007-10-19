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
****** REVISION HISTORY
******
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/beamline.h>
#include <beamline/sextupole.h>
#include <beamline/drift.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

using namespace std;


// **************************************************
//   class sextupole 
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole () : bmlnElmnt( 1.0, 0.0 ) {
 p_bml_ = BmlPtr( new beamline );
 p_bml_->append( DriftPtr( new drift( 1.0 / 2.0 ) ) );
 p_bml_->append( bml_e_ =  ThinSextupolePtr( new thinSextupole( 0.0 ) ) );
 p_bml_->append( DriftPtr( new drift( 1.0 / 2.0 ) ) );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

sextupole::sextupole ( double l, double s ) : bmlnElmnt( l, s ) {
 p_bml_ = BmlPtr( new beamline );
 p_bml_->append( DriftPtr( new drift( length_ / 2.0 ) ) );
 p_bml_->append( ThinSextupolePtr( new thinSextupole( strength_*length_ ) )); 
 p_bml_->append( DriftPtr( new drift( length_ / 2.0 ) ));

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


sextupole::sextupole ( const char* n, double l, double s ) : bmlnElmnt( n, l, s ) {
 p_bml_ = BmlPtr( new beamline );
 p_bml_->append( DriftPtr( new drift( length_ / 2.0 ) ));
 p_bml_->append( bml_e_ = ThinSextupolePtr( new thinSextupole( strength_*length_ ) )); 
 p_bml_->append( DriftPtr( new drift( length_ / 2.0 ) ));

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
  strength_ = s - getShunt()*IToField();
  bml_e_->setStrength( strength_*length_ );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::setCurrent( double s ) {
 bml_e_->setCurrent( s );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* sextupole::Type() const 
{ 
  return "sextupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "pc = " << pc << ": this should be within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void sextupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const",
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  a = SextupolePtr( new sextupole(         pc  *length_, strength_ ) );
  b = SextupolePtr( new sextupole( ( 1.0 - pc )*length_, strength_ ) );

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  //   : the entire XXX::Split strategy should be/is being overhauled.
  a->setAlignment( Alignment() );
  b->setAlignment( Alignment() );

  // Rename
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool sextupole::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void sextupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
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

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinSextupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}


