/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      DriftsToSlots.cc
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
 *  Leo Michelotti
 *  
 */


#include "beamline.h"
#include "CF_rbend.h"
#include "Slot.h"

using namespace std;

char d2S_LookUpStream ( bmlnElmnt*&      el2Ptr, 
                        char&            rb, 
                        char&            CF, 
                        BeamlineIterator bi
                       )
{
  rb = 0;
  CF = 0;

  bi.goBack();
  bi.goBack();
  el2Ptr = bi++;

  // cout << "DGN> " 
  //      << el2Ptr->Type() << "  " 
  //      << el2Ptr->Name()
  //      << endl;

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )     ) 
  {
    // REMOVE: if( 0.0 != el2Ptr->Length() ) {
    // REMOVE:   return 0;
    // REMOVE: }

    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return 0;
    }

    bi.goBack();
    bi.goBack();
    el2Ptr = bi++;
    // cout << "DGN> " 
    //      << el2Ptr->Type() << "  " 
    //      << el2Ptr->Name()
    //      << endl;

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
    return 0;
  }

  return 1;
}




char d2S_LookDownStream ( bmlnElmnt*&      el2Ptr, 
                          char&            rb, 
                          char&            CF, 
                          BeamlineIterator bi
                       )
{
  rb = 0;
  CF = 0;

  el2Ptr = bi++;
  if( el2Ptr == 0 ) {
    bi.reset();
    el2Ptr = bi++;
  }

  // cout << "DGN> " 
  //      << el2Ptr->Type() << "  " 
  //      << el2Ptr->Name()
  //      << endl;

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )     ) 
  {
    // REMOVE: if( 0.0 != el2Ptr->Length() ) {
    // REMOVE:   return 0;
    // REMOVE: }

    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return 0;
    }

    el2Ptr = bi++;
    if( el2Ptr == 0 ) {
      bi.reset();
      el2Ptr = bi++;
    }
    // cout << "DGN> " 
    //      << el2Ptr->Type() << "  " 
    //      << el2Ptr->Name()
    //      << endl;

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
    return 0;
  }

  return 1;
}




beamline* DriftsToSlots( /* const */ beamline& original )
{
  bmlnElmnt* elPtr;
  bmlnElmnt* el2Ptr;
  dlink*     linkPtr;
  dlink*     link2Ptr;


  // Check if Slots are already present ...
  DeepBeamlineIterator dbi( original );
  while((  elPtr = dbi++  )) 
  {
    if( 0 == strcmp( elPtr->Type(), "Slot" ) ) {
      cerr << "*** ERROR ***                                        \n"
              "*** ERROR *** DriftsToSlots                          \n"
              "*** ERROR *** Slots already exist in this beamline.  \n"
              "*** ERROR ***                                        \n"
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

  char isUpStream, isDownStream;
  char a_rb, a_CF, c_rb, c_CF;
  bmlnElmnt* a;
  bmlnElmnt* c;
  

  // Process the flattened beamline ...
  BeamlineIterator bi( flatRing );
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
      // There are four possibilities
      // cout << "DGN> Looking upstream for "
      //      << elPtr->Type() << "  "
      //      << elPtr->Name() << "  "
      //      << endl;
      isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CF, bi );
      // cout << "DGN> Looking downstream for "
      //      << elPtr->Type() << "  "
      //      << elPtr->Name() << "  "
      //      << endl;
      isUpStream    = d2S_LookDownStream ( c, c_rb, c_CF, bi );

      // isUpStream   = ( ( c_rb = ( 0 == strcmp( c->Type(), "rbend"    ) ) ) || 
      //                  ( c_CF = ( 0 == strcmp( c->Type(), "CF_rbend" ) ) )
      //                );
      // isDownStream = ( ( a_rb = ( 0 == strcmp( a->Type(), "rbend"    ) ) ) || 
      //                  ( a_CF = ( 0 == strcmp( a->Type(), "CF_rbend" ) ) )
      //                );
      // 
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
          exit(22);
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
          exit(22);
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
          exit(22);
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
          exit(22);
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

  ret->setEnergy( original.Energy() );
  ret->Rename( original.Name() );
  return ret;
}
