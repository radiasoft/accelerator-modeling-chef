/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      DriftsToSlots.cc
******  Version:   3.0
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
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


/*
 *  DriftsToSlots
 *  
 *  Utility to replace all drifts with Slots when
 *  they are either upstream and/or downstream
 *  of rbends of CF_rbends.
 *  
 *  Version 1.0
 *  October 30, 1998
 *  
 *
 *  Changed the algorithm to be more general.
 *  It now searches upstream and downstream for
 *  zero-length elements or rbends.
 *
 *  Version 2.0
 *  September 14, 1999
 *
 *
 *  Added tests to check whether the beamline is
 *  or is not to be treated as a ring.
 *
 *  Version 2.1
 *  January 20, 2005
 *
 * 
 *  Extended to include the possibility of 
 *  [CF]_sbend magnets with parallel faces.
 * 
 *  WARNING: UNTESTED!!
 *
 *  Version 3.0
 *  February 24, 2005
 *
 *  Leo Michelotti
 *  
 */


#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "GenericException.h"
#include "beamline.h"
#include "CF_rbend.h"
#include "Slot.h"

using namespace std;

bool d2S_LookUpStream ( bmlnElmnt*&      el2Ptr, 
                        bool&            rb, 
                        bool&            CFrb, 
                        bool&            sb, 
                        bool&            CFsb, 
                        BeamlineIterator bi
                      )
{
  rb   = false;
  CFrb = false;
  sb   = false;
  CFsb = false;

  bi.goBack();
  bi.goBack();
  el2Ptr = bi++;

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )  &&
         0 != strcmp( el2Ptr->Type(), "sbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }
    if( 0.0 != el2Ptr->Length() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "beamline* DriftsToSlots( beamline& original )", 
             "This routine works when only thin elements lie between"
             " drifts and bends." ) );
    }

    bi.goBack();
    bi.goBack();
    el2Ptr = bi++;
  }

  rb   = ( 0 == strcmp( el2Ptr->Type(), "rbend"    ) );
  CFrb = ( 0 == strcmp( el2Ptr->Type(), "CF_rbend" ) );
  sb   = ( 0 == strcmp( el2Ptr->Type(), "sbend"    ) );
  CFsb = ( 0 == strcmp( el2Ptr->Type(), "CF_sbend" ) );

  if( !(rb || CFrb || sb || CFsb ) ) 
  {
      throw( GenericException( __FILE__, __LINE__, 
             "beamline* DriftsToSlots( beamline& original )", 
             "A horrible, inexplicable error has"
             "  occurred while looking upstream." ) );
    return false;
  }

  return true;
}




bool d2S_LookDownStream ( bmlnElmnt*&      el2Ptr, 
                          bool&            rb, 
                          bool&            CFrb, 
                          bool&            sb, 
                          bool&            CFsb, 
                          BeamlineIterator bi
                        )
{
  rb   = false;
  CFrb = false;
  sb   = false;
  CFsb = false;

  el2Ptr = bi++;
  if( el2Ptr == 0 ) {
    bi.reset();
    el2Ptr = bi++;
  }

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )  &&
         0 != strcmp( el2Ptr->Type(), "sbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }
    if( 0.0 != el2Ptr->Length() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "beamline* DriftsToSlots( beamline& original )", 
             "This routine works when only thin elements lie between"
             " drifts and bends." ) );
    }

    el2Ptr = bi++;
    if( el2Ptr == 0 ) {
      bi.reset();
      el2Ptr = bi++;
    }
  }

  rb   = ( 0 == strcmp( el2Ptr->Type(), "rbend"    ) );
  CFrb = ( 0 == strcmp( el2Ptr->Type(), "CF_rbend" ) );
  sb   = ( 0 == strcmp( el2Ptr->Type(), "sbend"    ) );
  CFsb = ( 0 == strcmp( el2Ptr->Type(), "CF_sbend" ) );

  if( !(rb || CFrb || sb || CFsb ) ) 
  {
    if( 0.0 != el2Ptr->Length() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "beamline* DriftsToSlots( beamline& original )", 
             "A horrible, inexplicable error has"
             "  occurred while looking downstream." ) );
    }
    return false;
  }

  return true;
}


bool d2S_rbendLike( const bmlnElmnt* xPtr )
{
  if(0 == strcmp("rbend", xPtr->Type())) {
    const rbend* bp = dynamic_cast<const rbend*>(xPtr);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("CF_rbend", xPtr->Type())) {
    const CF_rbend* bp = dynamic_cast<const CF_rbend*>(xPtr);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("sbend", xPtr->Type())) {
    const sbend* bp = dynamic_cast<const sbend*>(xPtr);
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryEdgeAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitEdgeAngle()  - halfBendAngle) < 1.0e-8)    );
  }
  if(0 == strcmp("CF_sbend", xPtr->Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>(xPtr);
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryEdgeAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitEdgeAngle()  - halfBendAngle) < 1.0e-8)      );
  }
  return false;
}


bool d2S_sbendLike( const bmlnElmnt* xPtr )
{
  if(0 == strcmp("sbend", xPtr->Type())) {
    const sbend* bp = dynamic_cast<const sbend*>(xPtr);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("CF_sbend", xPtr->Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>(xPtr);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  // REMOVE: if(0 == strcmp("rbend", xPtr->Type())) {
  // REMOVE:   const rbend* bp = dynamic_cast<const rbend*>(xPtr);
  // REMOVE:   if( (1.0e-8 < std::abs( bp->getEntryAngle() )) || 
  // REMOVE:       (1.0e-8 < std::abs( bp->getExitAngle()  ))    ) { 
  // REMOVE:     return false; 
  // REMOVE:   }
  // REMOVE:   return ( std::abs(bp->getEntryEdgeAngle() - bp->getEntryEdgeAngle()) < 1.0e-8 );
  // REMOVE:   // REMOVE: double halfBendAngle = ( bp->getEntryAngle() - bp->getExitAngle() )/ 2.0;
  // REMOVE:   // REMOVE: return ( (std::abs(bp->getEntryEdgeAngle() + halfBendAngle) < 1.0e-8) &&
  // REMOVE:   // REMOVE:          (std::abs(bp->getExitEdgeAngle()  + halfBendAngle) < 1.0e-8)    );
  // REMOVE: }
  // REMOVE: if(0 == strcmp("CF_rbend", xPtr->Type())) {
  // REMOVE:   const CF_rbend* bp = dynamic_cast<const CF_rbend*>(xPtr);
  // REMOVE:   return ( std::abs(bp->getEntryEdgeAngle() - bp->getEntryEdgeAngle()) < 1.0e-8 );
  // REMOVE: }
  return false;
}


beamline* DriftsToSlots( /* const */ beamline& original )
{
  // This routine creates and returns a beamline
  //   for which the invoker must take responsibility.

  bmlnElmnt* elPtr;
  bmlnElmnt* el2Ptr;
  const bmlnElmnt* firstPtr = original.firstElement();
  const bmlnElmnt* lastPtr  = original.lastElement();


  // There should be more than one element
  if( firstPtr == lastPtr ) {
    cerr << "*** WARNING ***                                        \n"
            "*** WARNING *** DriftsToSlots                          \n"
            "*** WARNING *** Beamline contains only one element.    \n"
            "*** WARNING *** Original line will be returned.        \n"
            "*** WARNING ***                                        \n"
         << endl;
    return &original;
  }


  // Check if Slots are already present ...
  DeepBeamlineIterator dbi( original );
  while((  elPtr = dbi++  )) 
  {
    if( 0 == strcmp( elPtr->Type(), "Slot" ) ) {
      cerr << "*** WARNING ***                                        \n"
              "*** WARNING *** DriftsToSlots                          \n"
              "*** WARNING *** Slots already exist in this beamline.  \n"
              "*** WARNING *** Original line will be returned.        \n"
              "*** WARNING ***                                        \n"
           << endl;
      return &original;
    }
  }


  // Create new beamlines ...
  beamline* ret = new beamline;
  beamline* flatRing = original.flatten();


  // Some variables ...
  Frame arcFrame;
  Vector fd(3);
  enum { x = 0, y, z };
  arcFrame.reset();  

  bool isUpStream, isDownStream;
  bool a_rb, a_CFrb, a_sb, a_CFsb;
  bool c_rb, c_CFrb, c_sb, c_CFsb;
  bmlnElmnt* a;
  bmlnElmnt* c;
  

  // Process the flattened beamline ...
  BeamlineIterator bi( flatRing );
  bool isRing = ( beamline::ring == flatRing->getLineMode() );
  firstPtr = flatRing->firstElement();
  lastPtr  = flatRing->lastElement();

  while((  elPtr = bi++  )) 
  {
    // If the trial element is not a drift, just copy it ...
    if( 0 !=  strcmp( elPtr->Type(), "drift" ) ) 
    {
      ret->append( elPtr->Clone() );
    }
    // ... otherwise, begin processing ...
    else 
    {
      // Search upstream and downstream for the closest rbend, sbend, 
      // CF_rbend, or CF_sbend
      try {
      	if( isRing ) {
      	  isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, bi );
      	  isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, bi );
      	}
      	else {
      	  if( firstPtr == elPtr ) {
      	    isDownStream  = false;
      	    isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, bi );
      	  }
      	  else if( lastPtr == elPtr ) {
      	    isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, bi );
      	    isUpStream    = false;
      	  }
      	  else {
      	    isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, bi );
      	    isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, bi );
      	  }
      	}
      }
      catch( const std::exception& ge ) {
        ostringstream uic;
        uic << "File: " << "  " << __FILE__ << ", line " << __LINE__ << ": "
            << "Exception was thrown by DriftsToSlots.\n"
               "The message was:\n"
            << ge.what();
        cerr << "\n--- BEGIN EXCEPTION ---\n" 
             << uic 
             << "\n--- END EXCEPTION -----\n" 
             << endl;
        delete flatRing;
        ret->eliminate();
        return &original;
      }


      // Check viability of continuing the calculation
      if( isDownStream ) {
        if( ( a_rb || a_CFrb ) && !d2S_rbendLike( a ) ) {
          cerr << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation discontinued."
               << "\n*** WARNING: *** Assumptions regarding rbends violated."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete flatRing;
          ret->eliminate();
          return &original;
	}
      }
      if( isUpStream ) {
        if( ( c_rb || c_CFrb ) && !d2S_rbendLike( c ) ) {
          cerr << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation discontinued."
               << "\n*** WARNING: *** Assumptions regarding rbends violated."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete flatRing;
          ret->eliminate();
          return &original;
	}
      }


      // There are four possibilities
      if( !isUpStream && !isDownStream )
      {
        ret->append( elPtr->Clone() );
      }
  
      else if( isUpStream && !isDownStream )
      {
        if( d2S_sbendLike( c ) ) {
          ret->append( elPtr->Clone() );
	}
        else if( d2S_rbendLike(c) ) {
          arcFrame.reset();  
          if( c_sb ) {
            arcFrame.rotate( - ((sbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( c_CFsb ) {
            arcFrame.rotate( - ((CF_sbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
          }
          if( c_rb ) {
            arcFrame.rotate( - ((rbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( c_CFrb ) {
            arcFrame.rotate( - ((CF_rbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else {
            cerr << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Both c_rb and c_CF are false."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete flatRing;
            ret->eliminate();
            return &original;
          }
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          ret->append( new Slot(elPtr->Name(), arcFrame) );
	}
        else {
          cerr << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation cannot be continued."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete flatRing;
          ret->eliminate();
          return &original;
	}
      }
  
      else if( !isUpStream && isDownStream )
      {
        if( d2S_sbendLike( a ) ) {
          ret->append( elPtr->Clone() );
	}
        else if( d2S_rbendLike( a ) ) {
          arcFrame.reset(); 
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          if( a_sb ) {
            arcFrame.rotate( - ((sbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( a_CFsb ) {
            arcFrame.rotate( - ((CF_sbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
          }
          if( a_rb ) {
            arcFrame.rotate( - ((rbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( a_CFrb ) {
            arcFrame.rotate( - ((CF_rbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
          }
          else {
            cerr << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Both a_rb and a_CF are false."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete flatRing;
            ret->eliminate();
            return &original;
          }
          ret->append( new Slot(elPtr->Name(), arcFrame) );
	}
        else {
          cerr << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation cannot be continued."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete flatRing;
          ret->eliminate();
          return &original;
        }
      }
  
      else // isUpstream and isDownStream
      {
        if( d2S_sbendLike( a ) && d2S_sbendLike( c ) ) {
          ret->append( elPtr->Clone() );
	}
        else {
          arcFrame.reset();  
          if( d2S_rbendLike(c) ) {
            if( c_sb ) {
              arcFrame.rotate( - ((sbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( c_CFsb ) {
              arcFrame.rotate( - ((CF_sbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
            }
            if( c_rb ) {
              arcFrame.rotate( - ((rbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( c_CFrb ) {
              arcFrame.rotate( - ((CF_rbend*) c)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else {
              cerr << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Both c_rb and c_CF are false."
                   << "\n*** WARNING: *** "
                   << endl;
              delete flatRing;
              ret->eliminate();
              return &original;
            }
          }
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          if( d2S_rbendLike(a) ) {
            if( a_sb ) {
              arcFrame.rotate( - ((sbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( a_CFsb ) {
              arcFrame.rotate( - ((CF_sbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
            }
            if( a_rb ) {
              arcFrame.rotate( - ((rbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( a_CFrb ) {
              arcFrame.rotate( - ((CF_rbend*) a)->getEntryAngle(), arcFrame.getyAxis() );
            }
            else {
              cerr << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Both a_rb and a_CF are false."
                   << "\n*** WARNING: *** "
                   << endl;
              delete flatRing;
              ret->eliminate();
              return &original;
            }
	  }
          ret->append( new Slot(elPtr->Name(), arcFrame) );
	}
      }
  
    } // End processing element
  
  } // End while loop
  
  
  // Get rid of flatRing
  // Because the original beamline is flattened
  // but not cloned, do not use .zap() 
  // or .eliminate()
  delete flatRing;


  // Final manipulations
  ret->setEnergy( original.Energy() );
  ret->Rename( original.Name() );
  ret->setLineMode( original.getLineMode() );
  return ret;
}
