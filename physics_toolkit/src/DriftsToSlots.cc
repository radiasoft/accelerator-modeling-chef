/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Physics TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
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
****** REVISION HISTORY
****** ----------------
****** Jan 2008   michelotti@fnal.gov
******  - For the purpose of this note, a "regular bend"
******    is one whose faces are either normal entry or parallel;
******    otherwise, I'll call it "irregular."
******  - Until now, DriftsToSlots worked only with regular bends.
******    I'm generalizing it to work with irregular bends as well.
******  - WARNING: ONLY PARTLY DONE!!  This version still fails
******    : (a) for non-symmetric irregular bends: i.e. those that
******      have DIFFERENT entry and exit edge angles.
******      : (the closed orbit gets distorted for some reason)
******    : (b) possibly, if an irregular bend is rolled.
******    : (c) when a drift is both upstream and downstream of
******      an irregular bend.
******      : (a ring does not close)
******
****** Oct 2007   michelotti@fnal.gov
******  - a line that had been removed at some point in the murky
******    past is being returned
******  - extended functionality to dipoles that bend vertically
****** 
****** Mar 2007   ostiguy@fnal.gov
******  - reference counted elements/beamlines 
******  - eliminated references to slist/dlist
******  - use new-style STL compatible beamline iterators
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

#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/rbend.h>
#include <beamline/sbend.h>
#include <beamline/beamline.h>
#include <beamline/CF_rbend.h>
#include <beamline/CF_sbend.h>
#include <beamline/Slot.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool d2S_LookUpStream ( ElmPtr&              el2Ptr, 
                        bool&                rb, 
                        bool&                CFrb, 
                        bool&                sb, 
                        bool&                CFsb, 
                        beamline::iterator   bi
                      )
{

  rb   = false;
  CFrb = false;
  sb   = false;
  CFsb = false;

  el2Ptr = *(--bi);

  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )  &&
         0 != strcmp( el2Ptr->Type(), "sbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }

    if  (bi == bi.begin() ) return false;

    el2Ptr = *(--bi); 
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
  }

  return true;
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool d2S_LookDownStream ( ElmPtr&            el2Ptr, 
                          bool&              rb, 
                          bool&              CFrb, 
                          bool&              sb, 
                          bool&              CFsb, 
                          beamline::iterator bi
                        )
{
  rb   = false;
  CFrb = false;
  sb   = false;
  CFsb = false;

  ++bi;
  el2Ptr = ( bi != bi.end() ) ? (*bi) : *(bi.begin());


  while( 0 != strcmp( el2Ptr->Type(), "rbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_rbend" )  &&
         0 != strcmp( el2Ptr->Type(), "sbend"    )  &&
         0 != strcmp( el2Ptr->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", el2Ptr->Type() ) ) {
      return false;
    }

  ++bi;
  el2Ptr = ( bi != bi.end() ) ? (*bi) : *(bi.begin());

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


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool d2S_rbendLike( bmlnElmnt& x)
{
  if(0 == strcmp("rbend", x.Type())) {
    rbend const* bp = static_cast<rbend *>( &x );
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  if(0 == strcmp("CF_rbend", x.Type())) {
    CF_rbend const* bp = static_cast<CF_rbend *>( &x );
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  if(0 == strcmp("sbend", x.Type())) {
    sbend const* bp = static_cast<sbend *>( &x );
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryFaceAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitFaceAngle()  - halfBendAngle) < 1.0e-8)    );
  }
  if(0 == strcmp("CF_sbend", x.Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>(&x);
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryFaceAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitFaceAngle()  - halfBendAngle) < 1.0e-8)      );
  }
  return false;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool d2S_sbendLike( bmlnElmnt const& x )
{
  if(0 == strcmp("sbend", x.Type())) {
    const sbend* bp = dynamic_cast<const sbend*>(&x);
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  if(0 == strcmp("CF_sbend", x.Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>( &x);
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  return false;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


beamline* DriftsToSlots( beamline const& argbml )
{
  beamline original(argbml);  

  double entryAngle = 0.0, exitAngle  = 0.0, rollAngle  = 0.0;
  // Not necessary to initialize these, but what the hell.

  // Preliminary tests for a valid, sane argument.
  const bool originalRing = ( beamline::ring == original.getLineMode() );

  // There should be more than one element
  if( original.countHowManyDeeply() < 3 ) {
    (*pcerr) << "*** WARNING ***                                        \n"
         << "*** WARNING *** File: " << "  " << __FILE__ 
         <<                               ", line " << __LINE__ << "\n"
            "*** WARNING *** DriftsToSlots                          \n"
            "*** WARNING *** Beamline contains only "
         << (original.countHowManyDeeply()) <<       " elements.    \n"
            "*** WARNING *** Original line will be returned.        \n"
            "*** WARNING ***                                        \n"
         << endl;
    return original.Clone();  
  }

  // Bends with parallel faces should not be adjacent.
  // 

  ElmPtr q;
  bool prev_is_rbend_like = false; // true if the previous element is rbend-like

  for ( beamline::deep_iterator dit = original.deep_begin(); dit != original.deep_end(); ++dit )
  {
    q = *dit;
 
    if( d2S_rbendLike( *q ) ) {

      if( prev_is_rbend_like ) {
 
       (*pcerr) << "\n*** WARNING ***                                        "
             << "\n*** WARNING *** File: " << "  " << __FILE__ 
             <<                          ", line " << __LINE__
             << "\n*** WARNING *** DriftsToSlots                          "
                "\n*** WARNING *** Adjacent bends with parallel faces is forbidden."
                "\n*** WARNING *** Original line will be returned.        "
                "\n*** WARNING ***                                        "
             << endl;
        return  original.Clone();
      } else {
         prev_is_rbend_like  = true;
      }
    } else {  // d2S_rbendLike( *q ) 

      prev_is_rbend_like  = false;
    }
  }  

  // ----------------------------------------------------------------------------------------------------------
  // if we come out of the above loop with  prev_is_rbend_like = true, this means that the last element
  // was rbend like. In that case, if we are dealing with a ring, we need to test that the "next" (i.e. first) 
  // element, is also not  rbend like ...
  //----------------------------------------------------------------------------------------------------------- 

  if( prev_is_rbend_like && originalRing ) {
    if( d2S_rbendLike( **original.deep_begin() )) {
        (*pcerr) << "\n*** WARNING ***                                        "
             << "\n*** WARNING *** File: " << "  " << __FILE__ 
             <<                          ", line " << __LINE__
             << "\n*** WARNING *** DriftsToSlots                          "
                "\n*** WARNING *** Adjacent bends with parallel faces is forbidden."
                "\n*** WARNING *** Original line will be returned.        "
                "\n*** WARNING ***                                        "
             << endl;
        return  original.Clone();
    }
  }


  // Tests passed. Proceeding ...

  ElmPtr elPtr;
  ElmPtr el2Ptr;

  //-----------------------------------------
  // Check if Slots are already present ...
  //-----------------------------------------

  for ( beamline::deep_iterator dit = original.deep_begin(); dit != original.deep_end(); ++dit )
  {
    elPtr = *dit;

    if( 0 == strcmp( elPtr->Type(), "Slot" ) ) {
      (*pcerr) << "*** WARNING ***                                        \n"
              "*** WARNING *** DriftsToSlots                          \n"
              "*** WARNING *** Slots already exist in this beamline.  \n"
              "*** WARNING *** Original line will be returned.        \n"
              "*** WARNING ***                                        \n"
           << endl;
      return  original.Clone();
    }
  }


  // Create new beamlines ...
 
  beamline* ret = new beamline;
  beamline  flatRing = original.flatten();


  // Some variables ...
  Frame arcFrame;
  Vector fd(3);
  enum { x = 0, y, z };
  arcFrame.reset();  

  bool isUpStream, isDownStream;
  
  bool a_rb, a_CFrb, a_sb, a_CFsb;
  bool c_rb, c_CFrb, c_sb, c_CFsb;

  ElmPtr a;
  ElmPtr c;
  

  // Process the flattened beamline ...
  
  bool isRing = ( beamline::ring == flatRing.getLineMode() );


  beamline::iterator it2;
  for ( beamline::iterator it = flatRing.begin(); it != flatRing.end(); ++it )
  {
  
    elPtr = *it;

    bool isFirstPtr = (      it   == flatRing.begin() );
    it2 = it;
    bool isLastPtr  = ( ( ++it2)  == flatRing.end()   );

    // If the trial element is not a drift, just copy it ...
    if( 0 !=  strcmp( elPtr->Type(), "drift" ) ) 
    {
      ret->append( ElmPtr( elPtr->Clone() ) );
    }
    // ... otherwise, begin processing ...
    else 
    {
      // Search upstream and downstream for the closest rbend, sbend, 
      // CF_rbend, or CF_sbend
      try {
        if( isRing ) {
          isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, it );
          isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, it );
        }
        else {
          if( isFirstPtr ) {
            isDownStream  = false;
            isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, it );
          }
          else if( isLastPtr ) {
            isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, it );
            isUpStream    = false;
          }
          else {
            isDownStream  = d2S_LookUpStream   ( a, a_rb, a_CFrb, a_sb, a_CFsb, it );
            isUpStream    = d2S_LookDownStream ( c, c_rb, c_CFrb, c_sb, c_CFsb, it );
          }
        }
      }
      catch( const std::exception& ge ) {
        ostringstream uic;
        uic << "File: " << "  " << __FILE__ << ", line " << __LINE__ << ": "
            << "Exception was thrown within DriftsToSlots.\n"
               "The message was:\n"
            << ge.what();
        (*pcerr) << "\n--- BEGIN EXCEPTION ---\n" 
             << uic.str()
             << "\n Original argument will be returned."
             << "\n--- END EXCEPTION -----\n" 
             << endl;
        delete ret; ret=0;;
        return original.Clone();
      }


      // Check viability of continuing the calculation
      if( isDownStream ) {
        if( ( a_rb || a_CFrb ) && !d2S_rbendLike( *a ) ) {
          (*pcerr) << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation discontinued."
               << "\n*** WARNING: *** Assumptions regarding rbends violated."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete ret; ret=0;;
          return original.Clone();
        }
      }
      if( isUpStream ) {
        if( ( c_rb || c_CFrb ) && !d2S_rbendLike( *c ) ) {
          (*pcerr) << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** WARNING: *** Calculation discontinued."
               << "\n*** WARNING: *** Assumptions regarding rbends violated."
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete ret; ret=0;;
          return original.Clone();
        }
      }


      // There are four possibilities
      if( !isUpStream && !isDownStream )
      {
        ret->append( ElmPtr(elPtr->Clone()) );
      }
  
      else if( isUpStream && !isDownStream )
      {
        if( d2S_sbendLike( *c ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          // *** FIX ME *** DOES NOT WORK FOR ASYMMETRIC EDGE ANGLES ***
          if( c_sb ) {
            entryAngle = boost::static_pointer_cast<sbend>(c)->getEntryAngle();
          }
          else if( c_CFsb ) {
            entryAngle = boost::static_pointer_cast<CF_sbend>(c)->getEntryAngle();
          }
          else if( c_rb ) {
            entryAngle =  boost::static_pointer_cast<rbend>(c)->getEntryAngle();
          }
          else if( c_CFrb ) {
            entryAngle =  boost::static_pointer_cast<CF_rbend>(c)->getEntryAngle();
          }
          else {
            (*pcerr) << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete ret; ret=0;;
            return original.Clone();
          }
          rollAngle = c->Alignment().tilt;
          arcFrame.reset();
          if( 1.0e-12 < std::abs(rollAngle) ) {
            arcFrame.rotate(    rollAngle, arcFrame.getzAxis() );
            arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
            arcFrame.rotate( -  rollAngle, arcFrame.getzAxis() );
            // This last step is REALLY STUPID!
            // It is needed because
            // : it is too dangerous to remove the alignment object
            //   from the beamline element because
            //   : the same object may appear multiple times
            //     in the same beamline.
            // This can be fixed in the future by eliminating
            // the local alignment object.
          }
          else {
            arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
          }
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          ret->append( ElmPtr( new Slot(elPtr->Name().c_str(), arcFrame) ) );
        }
      }
  
      else if( !isUpStream && isDownStream )
      {
        if( d2S_sbendLike( *a ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          // *** FIX ME *** DOES NOT WORK FOR ASYMMETRIC EDGE ANGLES ***
          if( a_sb ) {
            exitAngle = boost::static_pointer_cast<sbend>(a)->getExitAngle();
          }
          else if( a_CFsb ) {
            exitAngle = boost::static_pointer_cast<CF_sbend>(a)->getExitAngle();
          }
          else if( a_rb ) {
            exitAngle = boost::static_pointer_cast<rbend>(a)->getExitAngle();
          }
          else if( a_CFrb ) {
            exitAngle = boost::static_pointer_cast<CF_rbend>(a)->getExitAngle();
          }
          else {
            (*pcerr) << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete ret; ret=0;;
            return original.Clone();
          }
          rollAngle = a->Alignment().tilt;
          arcFrame.reset(); 
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          if( 1.0e-12 < std::abs(rollAngle) ) {
            arcFrame.rotate(    rollAngle, arcFrame.getzAxis() );
            arcFrame.rotate(    exitAngle, arcFrame.getyAxis() );
            arcFrame.rotate( -  rollAngle, arcFrame.getzAxis() );
            // This last step is REALLY STUPID!
            // It is needed because
            // : it is too dangerous to remove the alignment object
            //   from the beamline element because
            //   : the same object may appear multiple times
            //     in the same beamline.
            // This can be fixed in the future by eliminating
            // the local alignment object.
          }
          else {
            arcFrame.rotate(    exitAngle, arcFrame.getyAxis() );
          }
          ret->append( ElmPtr( new Slot(elPtr->Name().c_str(), arcFrame) ) );
        }
      }
  
      else // isUpstream and isDownStream
      {
        if( d2S_sbendLike( *a ) && d2S_sbendLike( *c ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          // *** FIX ME *** THIS SECTION LOOKS WRONG!!! ***
          if( d2S_rbendLike(*c) ) {
            if( c_sb ) {
              entryAngle = boost::static_pointer_cast<sbend>(c)->getEntryAngle();
            }
            else if( c_CFsb ) {
              entryAngle = boost::static_pointer_cast<CF_sbend>(c)->getEntryAngle();
            }
            else if( c_rb ) {
              entryAngle = boost::static_pointer_cast<rbend>(c)->getEntryAngle();
            }
            else if( c_CFrb ) {
              entryAngle = boost::static_pointer_cast<CF_rbend>(c)->getEntryAngle();
            }
            else {
              (*pcerr) << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Function will return original argument."
                   << "\n*** WARNING: *** "
                   << endl;
              delete ret; ret=0;;
              return original.Clone();
            }
          }
          rollAngle = c->Alignment().tilt;
          arcFrame.reset();  
          if( 1.0e-12 < std::abs(rollAngle) ) {
            arcFrame.rotate(    rollAngle, arcFrame.getzAxis() );
            arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
            arcFrame.rotate( -  rollAngle, arcFrame.getzAxis() );
            // This last step is REALLY STUPID!
            // It is needed because
            // : it is too dangerous to remove the alignment object
            //   from the beamline element because
            //   : the same object may appear multiple times
            //     in the same beamline.
            // This can be fixed in the future by eliminating
            // the local alignment object.
          }
          else {
            arcFrame.rotate( - entryAngle, arcFrame.getyAxis() );
          }

          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);

          if( d2S_rbendLike( *a) ) {
            if( a_sb ) {
              exitAngle = boost::static_pointer_cast<sbend>(a)->getExitAngle();
            }
            else if( a_CFsb ) {
              exitAngle = boost::static_pointer_cast<CF_sbend>(a)->getExitAngle();
            }
            else if( a_rb ) {
              exitAngle = boost::static_pointer_cast<rbend>(a)->getExitAngle();
            }
            else if( a_CFrb ) {
              exitAngle = boost::static_pointer_cast<CF_rbend>(a)->getExitAngle();
            }
            else {
              (*pcerr) << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Function will return original argument."
                   << "\n*** WARNING: *** "
                   << endl;
              delete ret; ret=0;
              return original.Clone();
            }
          }

          rollAngle = a->Alignment().tilt;
          if( 1.0e-12 < std::abs(rollAngle) ) {
            arcFrame.rotate(    rollAngle, arcFrame.getzAxis() );
            arcFrame.rotate(    exitAngle, arcFrame.getyAxis() );
            arcFrame.rotate( -  rollAngle, arcFrame.getzAxis() );
            // This last step is REALLY STUPID!
            // It is needed because
            // : it is too dangerous to remove the alignment object
            //   from the beamline element because
            //   : the same object may appear multiple times
            //     in the same beamline.
            // This can be fixed in the future by eliminating
            // the local alignment object.
          }
          else {
            arcFrame.rotate(    exitAngle, arcFrame.getyAxis() );
          }
          ret->append( ElmPtr(new Slot(elPtr->Name().c_str(), arcFrame)) );
        }
      }
  
    } // End processing element
  
  } // End for loop
  
 

  // Final manipulations
  ret->setEnergy( original.Energy() );
  ret->rename( original.Name() );
  ret->setLineMode( original.getLineMode() );

  return ret;
}
