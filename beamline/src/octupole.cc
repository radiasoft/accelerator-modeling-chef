/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      octupole.cc
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
******
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagators
****** 
****** Apr 2008           michelotti@fnal.gov
****** - added octupole::setLength(..) method to override
******   the base class implementation.
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator
****** - added explicit implementation for assigment operator.
****** 
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <beamline/octupole.h>
#include <beamline/OctupolePropagators.h>
#include <beamline/drift.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;


// **************************************************
//   class octupole
// **************************************************

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole::octupole() 
 : bmlnElmnt("", 1.0, 0.0) 
{
  propagator_ = PropagatorPtr( new Propagator());  
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole::octupole( std::string const& n, double const& l, double const& s ) 
: bmlnElmnt( n, l, s ) 
{ 
  propagator_ = PropagatorPtr( new Propagator() );  
  propagator_->setup(*this);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole::octupole( octupole const& x ) 
  : bmlnElmnt( x )
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole& octupole::operator=( octupole const& rhs)
{

  if (& rhs == this) return *this;

  bmlnElmnt::operator=(rhs);

  return *this;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

octupole::~octupole() 
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* octupole::Type() const 
{ 
  return "octupole"; 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool octupole::isMagnet() const
{
  return true;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw(  GenericException( __FILE__, __LINE__, 
           "void octupole::Split( double const& pc, bmlnElmnt** a, bmlnElmnt** b )", 
           uic.str().c_str() ) );
  }

  // We assume "strength" means field, not field*length_.
  a = OctupolePtr( Clone() );
  b = OctupolePtr( Clone() );

  a->setLength(          pc* length_ );
  b->setLength( (1.0 - pc) * length_ );

  // Set the alignment struct
  // : this is a STOPGAP MEASURE!!!
  // -----------------------------------------------------------------
  a->setAlignment( Alignment() );
  b->setAlignment( Alignment() );

  // Rename
  a->rename( ident_ + string("_1") );
  b->rename( ident_ + string("_2") );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void octupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// **************************************************
//   class thinOctupole
// **************************************************

thinOctupole::thinOctupole () 
: bmlnElmnt( "", 0.0, 0.0 ) 
{
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
  propagator_ = PropagatorPtr( new Propagator() );  
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole::thinOctupole ( std::string const& n, double const& s ) 
 : bmlnElmnt( n, 0.0, s ) 
{
 // The strength is to be interpreted as
 // (1/3!)*B'''l  in  Tesla-meters^-2
  propagator_ = PropagatorPtr( new Propagator() );  
  propagator_->setup(*this);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole::thinOctupole( thinOctupole const& x ) 
  : bmlnElmnt( x )
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole& thinOctupole::operator=( thinOctupole const& rhs)
{
  if (&rhs == this) return *this;
  bmlnElmnt::operator=(rhs);
  return *this;
}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

thinOctupole::~thinOctupole() 
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* thinOctupole::Type() const 
{ 
  return "thinOctupole"; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool thinOctupole::isMagnet() const
{
  return true;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::accept( BmlVisitor& v ) 
{ 
  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void thinOctupole::accept( ConstBmlVisitor& v ) const 
{ 
  v.visit( *this ); 
}

