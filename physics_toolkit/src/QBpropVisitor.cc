#include "beamline.h"
#include "QBpropVisitor.h"

QBpropVisitor::QBpropVisitor( const Particle& x )
: particleVisitor( x )
{
}


QBpropVisitor::QBpropVisitor( const QBpropVisitor& x )
: particleVisitor( (const particleVisitor&) x )
{
}

QBpropVisitor::~QBpropVisitor()
{
}



void QBpropVisitor::visitBmlnElmnt( bmlnElmnt* x )
{
  static drift OO( 1. );
  static double lng;
  if( 0.0 != ( lng = x->Length() ) )  {
    OO.setLength( lng );
    OO.propagate( *particle );
  }
}


void QBpropVisitor::visitMarker( marker* x )
{
  // Do nothing.
}


void QBpropVisitor::visitDrift( drift* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitRbend( rbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSbend( sbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSector( sector* x )
{
  cerr << "*** ERROR ***                              \n"
          "*** ERROR *** QBpropVisitor::visitSector   \n"
          "*** ERROR *** Sorry. Sectors not allowed.  \n"
          "*** ERROR ***                              \n"
       << endl;
  exit(1);
}


void QBpropVisitor::visitQuadrupole( quadrupole* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitJetQuadrupole( JetQuadrupole* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitThinQuad( thinQuad* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitSlot( Slot* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::visitCF_rbend( CF_rbend* x )
{
  x->propagate( *particle );
}


void QBpropVisitor::getState( Vector& x )
{
  particle->getState( x );
}


void QBpropVisitor::getState( Vector* x )
{
  particle->getState( x );
}


void QBpropVisitor::getState( double* x )
{
  particle->getState( x );
}

Vector QBpropVisitor::State()
{
  return particle->State();
}
