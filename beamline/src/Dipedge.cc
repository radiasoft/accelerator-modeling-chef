/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Dipedge.cc
******
******  Copyright Universities Research Association, Inc., Fermilab
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
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
****** REVISION HISTORY
****** ----------------
******
****** Apr 2013           Leo Michelotti
******                    michelotti@fnal.gov
****** - initial implementation, complete with errors.
******   THIS IS ONLY A PROTOTYPE VERSION, little
******   more than a placeholder file. 
****** - class Dipedge is meant to mimic the DIPEDGE element
******   used by MAD-X.
****** - Associated files:
******     beamline/include/Dipedge.h
******     beamline/include/DipedgePropagators.h
******     beamline/src/DipedgePropagators.cc
******
**************************************************************************
*************************************************************************/

#include <beamline/Dipedge.h>
#include <beamline/DipedgePropagators.h>
#include <beamline/BmlVisitor.h>

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge::Dipedge()
: bmlnElmnt( "", 0.0, 0.0 )
{
  propagator_ = PropagatorPtr( new Propagator( /* with possible arguments */ ) );
  propagator_->setup( *this );  // if necessary
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge::Dipedge( char const* name, double const& length, double const& strength )
: bmlnElmnt( name, length, strength ),
  h_(0.0),
  edge_(0.0),
  fint_(0.0),
  hgap_(0.0),
  tilt_(0.0),
  corr_(0.0)
{
  propagator_ = PropagatorPtr( new Propagator( /* with possible arguments */ ) );
  propagator_->setup( *this );  // if necessary
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge::Dipedge( char const* name, double const& length, double const& strength,
                  double const& h, 
                  double const& e1,
                  double const& fint,
                  double const& hgap,
                  double const& tilt )
: bmlnElmnt( name, length, strength ),
  h_(h),
  edge_(e1),
  fint_(fint),
  hgap_(hgap),
  tilt_(tilt), // ??? MUST BE HANDLED ???
  corr_( (h+h)*hgap*fint )
{
  propagator_ = PropagatorPtr( new Propagator( /* with possible arguments */ ) );
  propagator_->setup( *this );  // if necessary
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge::Dipedge( Dipedge const& x )
: bmlnElmnt(x),
  propagator_( PropagatorPtr( x.propagator_->Clone() ) ),
  h_(x.h_),
  edge_(x.edge_),
  fint_(x.fint_),
  hgap_(x.hgap_),
  tilt_(x.tilt_), // ??? MUST BE HANDLED ???
  corr_(x.corr_)
{
  // Lines added here if internal structure needs to be handled.
  // This should be avoided if at all possible. 
  // See (as yet non-existent) Design Notes.
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge* Dipedge::Clone() const
{
  // One line should be sufficient.

  return new Dipedge( *this );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Dipedge::~Dipedge()
{
  // Lines added here if internal structure needs to be disassembled.
  // This should be avoided if at all possible. 
  // See (as yet non-existent) Design Notes.
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::accept( BmlVisitor& v )            
{
  // One line is usually sufficient.

  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::accept( ConstBmlVisitor& v ) const 
{
  // One line is usually sufficient.

  v.visit( *this ); 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::localPropagate( Particle& p )   
{
  // One line is usually sufficient.

  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  Dipedge::localPropagate( JetParticle& p )   
{
  // One line is usually sufficient.

  (*propagator_)(*this, p);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void Dipedge::setStrength( double const& s )
{
  // Needed when internal structure requires
  // something more complicated than merely changing
  // the value of attribute bmlnElmnt::strength_
  // 
  // It should invoke the default behavior ...

  bmlnElmnt::setStrength(s);

  // ... in its first line.  
  // 
  // Afterwards, include any internal
  // manipulation as needed.  As example, see
  // quadrupole::setStrength in file
  // beamline/src/quadrupole.cc .
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Dipedge::isMagnet() const
{
  // Typically, this needs only one line, e.g. ...

  return true;
}
