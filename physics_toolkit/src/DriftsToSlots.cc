/*
 *  DriftsToSlots
 *  
 *  Utility to replace all drifts with Slots when
 *  they are either upstream and/or downstream
 *  of rbends of CF_rbends.
 *  
 *  Version 1.0  (untested)
 *  October 30, 1998
 *  
 *  Leo Michelotti
 *  
 */


#include "beamline.h"
#include "CF_rbend.h"
#include "Slot.h"


beamline* DriftsToSlots( /* const */ beamline& original )
{
  Frame arcFrame;
  Vector fd(3);
  enum { x = 0, y, z };
  arcFrame.reset();  

  beamline* ret = new beamline;
  beamline* flatRing = original.flatten();

  drift dummyDrift( 0.0 );
  flatRing->insert( dummyDrift );
  flatRing->append( dummyDrift );

  bmlnElmnt* a;
  bmlnElmnt* b;
  bmlnElmnt* c;
  
  dlist_iterator getNext((  *(dlist*) flatRing  ));
  
  char isUpStream, isDownStream;
  char a_rb, a_CF, c_rb, c_CF;
  
  a = (bmlnElmnt*) getNext();
  b = (bmlnElmnt*) getNext();
  
  // ret->append( a->Clone() );
  // Should not be necessary, since I've put a dummy drift
  // at the beginning of the original beamline
  
  while((  c = (bmlnElmnt*) getNext()  )) 
  {
    // Bypass markers.
    if( 0 == strcmp( c->Type(), "marker" ) ) continue;
  
    // If the trial element is not a drift, just copy it.
    if( 0 !=  strcmp( b->Type(), "drift" ) ) 
    {
      ret->append( b->Clone() );
    }
  
    // If the second element is a drift, then begin processing ...
    else 
    {
      // There are four possibilities
      isUpStream   = ( ( c_rb = ( 0 == strcmp( c->Type(), "rbend"    ) ) ) || 
		       ( c_CF = ( 0 == strcmp( c->Type(), "CF_rbend" ) ) )
		     );
      isDownStream = ( ( a_rb = ( 0 == strcmp( a->Type(), "rbend"    ) ) ) || 
		       ( a_CF = ( 0 == strcmp( a->Type(), "CF_rbend" ) ) )
		     );
  
      if( !isUpStream && !isDownStream )
      {
	ret->append( b->Clone() );
      }
  
      else if( isUpStream && !isDownStream )
      {
	arcFrame.reset();  
	if( c_rb ) {
	  arcFrame.rotate( - ((rbend*) c)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else if( c_CF ) {
	  arcFrame.rotate( - ((CF_rbend*) c)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else {
	  exit(22);
	}
	fd(x) = 0.0;
	fd(y) = 0.0;
	fd(z) = b->Length();
	arcFrame.translate(fd);
	ret->append( new Slot(b->Name(), arcFrame) );
      }
  
      else if( !isUpStream && isDownStream )
      {
	arcFrame.reset(); 
	fd(x) = 0.0;
	fd(y) = 0.0;
	fd(z) = b->Length();
	arcFrame.translate(fd);
	if( a_rb ) {
	  arcFrame.rotate( - ((rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else if( a_CF ) {
	  arcFrame.rotate( - ((CF_rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else {
	  exit(22);
	}
	ret->append( new Slot(b->Name(), arcFrame) );
      }
  
      else // isUpstream and isDownStream
      {
	arcFrame.reset();  
	if( c_rb ) {
	  arcFrame.rotate( - ((rbend*) c)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else if( c_CF ) {
	  arcFrame.rotate( - ((CF_rbend*) c)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else {
	  exit(22);
	}
	fd(x) = 0.0;
	fd(y) = 0.0;
	fd(z) = b->Length();
	arcFrame.translate(fd);
	if( a_rb ) {
	  arcFrame.rotate( - ((rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else if( a_CF ) {
	  arcFrame.rotate( - ((CF_rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
	}
	else {
	  exit(22);
	}
	ret->append( new Slot(b->Name(), arcFrame) );
      }
  
    } // End processing 
  
    // Get ready for next step
    a = b;
    b = c;
  
  } // End while loop
  
  
  // ret->append( b->Clone() );
  // Should not be necessary, since I've put a dummy drift
  // at the end of the original beamline.
  
  
  // Get rid of flatRing
  // Because the original beamline is flattened
  // but not cloned, do not use .zap() 
  // or .eliminate()
  
  delete flatRing;

  return ret;
}
