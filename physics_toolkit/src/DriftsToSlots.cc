/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Physics TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      DriftsToSlots.cc
******  Version:   3.1
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
****** REVISION HISTORY
******
****** Mar 2006   ostiguy@fnal.gov
******
******  - reference counted elements/beamlines 
******  - eliminated references to slist/dlist
******  - use new-style STL compatible beamline iterators
******
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

using namespace std;
using FNAL::pcout;
using FNAL::pcerr;


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


bool d2S_rbendLike( bmlnElmnt& x)
{
  if(0 == strcmp("rbend", x.Type())) {
    rbend const* bp = static_cast<rbend *>( &x );
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("CF_rbend", x.Type())) {
    CF_rbend const* bp = static_cast<CF_rbend *>( &x );
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("sbend", x.Type())) {
    sbend const* bp = static_cast<sbend *>( &x );
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryEdgeAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitEdgeAngle()  - halfBendAngle) < 1.0e-8)    );
  }
  if(0 == strcmp("CF_sbend", x.Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>(&x);
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryEdgeAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitEdgeAngle()  - halfBendAngle) < 1.0e-8)      );
  }
  return false;
}


bool d2S_sbendLike( bmlnElmnt const& x )
{
  if(0 == strcmp("sbend", x.Type())) {
    const sbend* bp = dynamic_cast<const sbend*>(&x);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }
  if(0 == strcmp("CF_sbend", x.Type())) {
    const CF_sbend* bp = dynamic_cast<const CF_sbend*>( &x);
    return ( (0.0 == bp->getEntryEdgeAngle()) && (0.0 == bp->getExitEdgeAngle()) );
  }

  return false;
}


beamline* DriftsToSlots( beamline const& argbml )
{

  beamline original(argbml);  

   // Preliminary tests for a valid argument.

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

  // -------------------------------------------------------------------------------------------
  // if we come out of the above loop with  prev_is_rbend_like = true, this means that the last element
  // was rbend like. In that case, if we are dealing with a ring, we need to test that the "next" (i.e. first) 
  // element, is also not  rbend like ...
  //------------------------------------------------------------------------------------------------ 

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


  // rbend-like elements can only be sandwiched by thin, 
  //   passive elements
  // bool passedRbend = false;
  // prev_is_rbend_like = false;
  // while((  q = dbi++  )) {
  //   if( d2S_rbendLike( q ) {
  //     if( prev_is_rbend_like ) {
  //    }
  //   }
  // }

  (*pcerr) << "\n*** WARNING ***                                        "
       << "\n*** WARNING *** File: " << "  " << __FILE__ 
       <<                          ", line " << __LINE__
       << "\n*** WARNING *** DriftsToSlots                          "
          "\n*** WARNING *** Second preliminary test not yet written."
          "\n*** WARNING *** Procedure is proceeding notwithstanding."
          "\n*** WARNING *** Keep your fingers crossed."
          "\n*** WARNING ***                                        "
       << endl;
  
  
  // Tests passed. Proceeding ...

  ElmPtr elPtr;
  ElmPtr el2Ptr;

  // Check if Slots are already present ...

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

  bool isFirstPtr = true;
  bool isLastPtr  = false;

  for ( beamline::iterator it = flatRing.begin(); it != flatRing.end(); ++it )
  {
  
    elPtr = *it;

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
            isFirstPtr    = false;
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
        else if( d2S_rbendLike(*c) ) {
          arcFrame.reset();  
          if( c_sb ) {
            arcFrame.rotate( - ((sbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( c_CFsb ) {
            arcFrame.rotate( - ((CF_sbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( c_rb ) {
            arcFrame.rotate( - ((rbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
          }
          else if( c_CFrb ) {
            arcFrame.rotate( - ((CF_rbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
          }
          else {
            (*pcerr) << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Both c_rb and c_CF are false."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete ret; ret=0;;
            return original.Clone();
          }
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          ret->append( ElmPtr( new Slot(elPtr->Name().c_str(), arcFrame) ) );
        }
        else {
          (*pcerr) 
               << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__
               << "\n*** WARNING: *** Calculation cannot be continued for "
               << elPtr->Type() << " " << elPtr->Name() << ": " << ((int) elPtr.get())
               << "\n*** WARNING: *** Upstream: "
               << a->Type() << " " << a->Name() << ": " << ((int) a.get())
               << "\n*** WARNING: *** Downstream: "
               << c->Type() << " " << c->Name() << ": " << ((int) c.get())
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete ret; ret=0;;
          return original.Clone();
        }
      }
  
      else if( !isUpStream && isDownStream )
      {
        if( d2S_sbendLike( *a ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else if( d2S_rbendLike( *a ) ) {
          arcFrame.reset(); 
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          if( a_sb ) {
            arcFrame.rotate( ((sbend*) (a.get()))->getExitAngle(), arcFrame.getyAxis() );
          }
          else if( a_CFsb ) {
            arcFrame.rotate( ((CF_sbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
          }
          else if( a_rb ) {
            arcFrame.rotate( ((rbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
          }
          else if( a_CFrb ) {
            arcFrame.rotate( ((CF_rbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
          }
          else {
            (*pcerr) << "\n*** WARNING: *** "
                 << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                 << "\n*** WARNING: *** Calculation discontinued."
                 << "\n*** WARNING: *** Both a_rb and a_CF are false."
                 << "\n*** WARNING: *** Function will return original argument."
                 << "\n*** WARNING: *** "
                 << endl;
            delete ret; ret=0;;
            return original.Clone();
          }
          ret->append( ElmPtr( new Slot(elPtr->Name().c_str(), arcFrame) ) );
        }
        else {
          (*pcerr) 
               << "\n*** WARNING: *** "
               << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__
               << "\n*** WARNING: *** Calculation cannot be continued for "
               << elPtr->Type() << " " << elPtr->Name() << ": " << ((int) elPtr.get())
               << "\n*** WARNING: *** Upstream: "
               << a->Type() << " " << a->Name() << ": " << ((int) a.get())
               << "\n*** WARNING: *** Downstream: "
               << c->Type() << " " << c->Name() << ": " << ((int) c.get())
               << "\n*** WARNING: *** Function will return original argument."
               << "\n*** WARNING: *** "
               << endl;
          delete ret; ret=0;;
          return original.Clone();
        }
      }
  
      else // isUpstream and isDownStream
      {
        if( d2S_sbendLike( *a ) && d2S_sbendLike( *c ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          arcFrame.reset();  
          if( d2S_rbendLike(*c) ) {
            if( c_sb ) {
              arcFrame.rotate( - ((sbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( c_CFsb ) {
              arcFrame.rotate( - ((CF_sbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( c_rb ) {
              arcFrame.rotate( - ((rbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
            }
            else if( c_CFrb ) {
              arcFrame.rotate( - ((CF_rbend*) c.get())->getEntryAngle(), arcFrame.getyAxis() );
            }
            else {
              (*pcerr) << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Both c_rb and c_CF are false."
                   << "\n*** WARNING: *** "
                   << endl;
              delete ret; ret=0;;
              return original.Clone();
            }
          }
          fd(x) = 0.0;
          fd(y) = 0.0;
          fd(z) = elPtr->Length();
          arcFrame.translate(fd);
          if( d2S_rbendLike( *a) ) {
            if( a_sb ) {
              arcFrame.rotate( ((sbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
            }
            else if( a_CFsb ) {
              arcFrame.rotate( ((CF_sbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
            }
            else if( a_rb ) {
              arcFrame.rotate( ((rbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
            }
            else if( a_CFrb ) {
              arcFrame.rotate( ((CF_rbend*) a.get())->getExitAngle(), arcFrame.getyAxis() );
            }
            else {
              (*pcerr) << "\n*** WARNING: *** "
                   << "\n*** WARNING: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                   << "\n*** WARNING: *** Calculation discontinued."
                   << "\n*** WARNING: *** Both a_rb and a_CF are false."
                   << "\n*** WARNING: *** "
                   << endl;
              delete ret; ret=0;
              return original.Clone();
            }
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
