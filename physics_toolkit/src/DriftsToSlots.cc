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
 *  non-zero longth elements or rbends.
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
    if( 0.0 != el2Ptr->Length() ) {
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
    if( 0.0 != el2Ptr->Length() ) {
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

  return ret;
}



// beamline* ConvertDriftsToSlots( /* const */ beamline& original, 
//                                             slist&    removedElements )
// {
//   Frame arcFrame;
//   Vector fd(3);
//   enum { x = 0, y, z };
//   arcFrame.reset();  
// 
//   beamline* ret = original.flatten();
// 
//   bmlnElmnt* elPtr;
//   bmlnElmnt* el2Ptr;
//   dlink*     linkPtr;
//   dlink*     link2Ptr;
//   dlist_traversor getNext((  *(dlist*) ret  ));
//   
//   bool       isNotDrift;
//   bool       isNotSlot;
//   while((  linkPtr = getNext()  )) 
//   {
//     // Process rbend and CF_rbend ...
//     elPtr = (bmlnElmnt*) linkPtr->e;
//     if( strcmp( elPtr->Type(), "rbend" )    == 0  ||
//         strcmp( elPtr->Type(), "CF_rbend" ) == 0
//       ) 
//     {
//       // Search for previous drift or Slot ...
//       link2Ptr = linkPtr->prev;
//       el2Ptr   = (bmlnElmnt*) link2ptr->e;
//       if( link2Ptr == linkPtr ) {
//         return ret;
//       }
//       while((isNotDrift = (strcmp( el2Ptr->Type, "drift" ) != 0))  &&
//             (isNotSlot  = (strcmp( el2Ptr->Type, "Slot" )  != 0))  )
//       {
//         if( strcmp( elPtr->Type(), "rbend" )    == 0  ||
//             strcmp( elPtr->Type(), "CF_rbend" ) == 0
// 	    ) 
//         {
//           cerr << "*** ERROR ***                                        \n"
//                   "*** ERROR *** ConvertDriftsToSlots                   \n"
//                   "*** ERROR *** Two rbend-like elements not separated  \n"
//                   "*** ERROR *** by a drift or Slot.                    \n"
//                   "*** ERROR ***                                        \n"
//                << endl;
//           return 0;
// 	}
//         link2Ptr = link2Ptr->prev;
//         el2Ptr   = (bmlnElmnt*) link2ptr->e;
//         if( link2Ptr == linkPtr ) {
//           return ret;
//         }
//       }
// 
//       // Process previous drift ...
//       if( !isNotDrift ) {
//         
//       }
// 
//       // Process previous Slot ...
//       else if( !isNotSlot ) {
//       }
// 
//       // Error
//       else {
//         cerr << "*** ERROR ***                                        \n"
//                 "*** ERROR *** ConvertDriftsToSlots                   \n"
//                 "*** ERROR *** A horrible, inexplicable error         \n"
//                 "*** ERROR *** has occurred. Call an exorcist now!    \n"
//                 "*** ERROR ***                                        \n"
//              << endl;
//         return 0;
//       }
// 
//     }
// 
// 
//   link2Ptr = linkPtr;  // For use outside while loop
//   }                    // End while loop
//   
//   return ret;
// }
