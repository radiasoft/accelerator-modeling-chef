/*
 * File: FramePusher.cc
 * 
 * class FramePusher
 * 
 * Moves a Frame around a beamline, 
 * in anticipation of drawing the footprint.
 *
 * Leo Michelotti
 * May 11, 2001
 *
 */


#include "FramePusher.h"
#include "bmlnElmnt.h"
#include "sbend.h"
#include "CF_sbend.h"
#include "Slot.h"

// Static error codes

const int FramePusher::OKAY          = 0;
const int FramePusher::SECTORVISITED = 1;


// Constructors

FramePusher::FramePusher()
: _errorCode(OKAY)
{
  Frame f;
  _frame = f;
}

FramePusher::FramePusher( const Frame& f )
: _frame(f), _errorCode(OKAY)
{
}

FramePusher::FramePusher( const FramePusher& x )
: _frame( x._frame ), _errorCode(OKAY)
{
}

FramePusher::~FramePusher()
{
}


// Visiting functions

void FramePusher::visitBmlnElmnt( bmlnElmnt* x )
{
  if( x->Length() > 0.0 ) {
    _frame.translate( (x->Length())*(_frame.getzAxis()) );
  }
}


void FramePusher::visitSbend    ( sbend* x    )
{
  static double angle;
  static double displacement;
  static double rho;

  angle = x->getAngle();
  rho = x->Length() / angle;
  angle /= 2.0;
  displacement = 2.0*rho*sin(angle);

  _frame.rotate( - angle, _frame.getyAxis(), false );
  _frame.translate( displacement*_frame.getzAxis() );
  _frame.rotate( - angle, _frame.getyAxis(), false );
}


void FramePusher::visitCF_sbend ( CF_sbend* x )
{
  static double angle;
  static double displacement;
  static double rho;

  angle = x->getAngle();
  rho = x->Length() / angle;
  angle /= 2.0;
  displacement = 2.0*rho*sin(angle);

  _frame.rotate( - angle, _frame.getyAxis(), false );
  _frame.translate( displacement*_frame.getzAxis() );
  _frame.rotate( - angle, _frame.getyAxis(), false );
}


void FramePusher::visitSector   ( sector*     )
{
  cerr << "*** WARNING ***                                \n"
       << "*** WARNING *** FramePusher::visitSector       \n"
       << "*** WARNING ***                                \n"
       << "*** WARNING *** Sectors are not handled        \n"
       << "*** WARNING *** properly. Errorcode is         \n"
       << "*** WARNING *** being set. Results will be     \n"
       << "*** WARNING *** unreliable.                    \n"
       << "*** WARNING ***                                \n"
       << endl;
  _errorCode = SECTORVISITED;
}


void FramePusher::visitSlot     ( Slot* x     )
{
  _frame = ( ( x->getOutFrame() ).relativeTo( x->getInFrame() ) ) 
           .patchedOnto( _frame );
}



