/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      DriftsToSlots.cc
******  Version:   2.1
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
 *  Version 1.0  (untested)
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
 *  Version 2.1 (untested)
 *  January 26, 2005
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
                        bool&            CF, 
                        BeamlineIterator bi
                      )
{
  rb = false;
  CF = false;

  bi.goBack();
  bi.goBack();
  el2Ptr = bi++;

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }

    bi.goBack();
    bi.goBack();
    el2Ptr = bi++;
  }

  rb = ( 0 == strcmp( el2Ptr->Type(), "rbend"    ) );
  CF = ( 0 == strcmp( el2Ptr->Type(), "CF_rbend" ) );

  if( !(rb || CF) ) 
  {
    cerr << "*** ERROR ***                                        \n"
            "*** ERROR *** ConvertDriftsToSlots                   \n"
            "*** ERROR *** A horrible, inexplicable error has     \n"
            "*** ERROR *** occurred while looking upstream.       \n"
            "*** ERROR ***                                        \n"
         << endl;
    return false;
  }

  return true;
}




bool d2S_LookDownStream ( bmlnElmnt*&      el2Ptr, 
                          bool&            rb, 
                          bool&            CF, 
                          BeamlineIterator bi
                        )
{
  rb = false;
  CF = false;

  el2Ptr = bi++;
  if( el2Ptr == 0 ) {
    bi.reset();
    el2Ptr = bi++;
  }

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }

    el2Ptr = bi++;
    if( el2Ptr == 0 ) {
      bi.reset();
      el2Ptr = bi++;
    }
  }

  rb = ( 0 == strcmp( el2Ptr->Type(), "rbend"    ) );
  CF = ( 0 == strcmp( el2Ptr->Type(), "CF_rbend" ) );

  if( !(rb || CF) ) 
  {
    cerr << "*** ERROR ***                                        \n"
            "*** ERROR *** ConvertDriftsToSlots                   \n"
            "*** ERROR *** A horrible, inexplicable error has     \n"
            "*** ERROR *** occurred while looking downstream.     \n"
            "*** ERROR ***                                        \n"
         << endl;
    return false;
  }

  return true;
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
  bool a_rb, a_CF, c_rb, c_CF;
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
      // Search upstream and downstream for the closest rbend or CF_rbend
      if( isRing ) {
        isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CF, bi );
        isUpStream    = d2S_LookDownStream ( c, c_rb, c_CF, bi );
      }
      else {
        if( firstPtr == elPtr ) {
          isDownStream  = false;
          isUpStream    = d2S_LookDownStream ( c, c_rb, c_CF, bi );
        }
        else if( lastPtr == elPtr ) {
          isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CF, bi );
          isUpStream    = false;
        }
        else {
          isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CF, bi );
          isUpStream    = d2S_LookDownStream ( c, c_rb, c_CF, bi );
        }
      }

      // There are four possibilities
      if( !isUpStream && !isDownStream )
      {
        ret->append( elPtr->Clone() );
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
          throw( GenericException( __FILE__, __LINE__, 
                 "beamline* DriftsToSlots( /* const */ beamline& original )", 
                 "Both c_rb and c_CF are null." ) );
        }
        fd(x) = 0.0;
        fd(y) = 0.0;
        fd(z) = elPtr->Length();
        arcFrame.translate(fd);
        ret->append( new Slot(elPtr->Name(), arcFrame) );
      }
  
      else if( !isUpStream && isDownStream )
      {
        arcFrame.reset(); 
        fd(x) = 0.0;
        fd(y) = 0.0;
        fd(z) = elPtr->Length();
        arcFrame.translate(fd);
        if( a_rb ) {
          arcFrame.rotate( - ((rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
        }
        else if( a_CF ) {
          arcFrame.rotate( - ((CF_rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
        }
        else {
          throw( GenericException( __FILE__, __LINE__, 
                 "beamline* DriftsToSlots( /* const */ beamline& original )", 
                 "Both a_rb and a_CF are null." ) );
        }
        ret->append( new Slot(elPtr->Name(), arcFrame) );
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
          throw( GenericException( __FILE__, __LINE__, 
                 "beamline* DriftsToSlots( /* const */ beamline& original )", 
                 "Both c_rb and c_CF are null." ) );
        }
        fd(x) = 0.0;
        fd(y) = 0.0;
        fd(z) = elPtr->Length();
        arcFrame.translate(fd);
        if( a_rb ) {
          arcFrame.rotate( - ((rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
        }
        else if( a_CF ) {
          arcFrame.rotate( - ((CF_rbend*) a)->getPoleFaceAngle(), arcFrame.getyAxis() );
        }
        else {
          throw( GenericException( __FILE__, __LINE__, 
                 "beamline* DriftsToSlots( /* const */ beamline& original )", 
                 "Both a_rb and a_CF are null." ) );
        }
        ret->append( new Slot(elPtr->Name(), arcFrame) );
      }
  
    } // End processing 
  
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
