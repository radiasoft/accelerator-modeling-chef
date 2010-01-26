/*************************************************************************
**************************************************************************
**************************************************************************
******
******  PHYSICS TOOLKIT: Library of utilites and Sage classes
******             which facilitate calculations with the
******             BEAMLINE class library.
******
******  File:      DriftConverter.cc
******
******             Implementation of class DriftConverter
******             - automatically sandwiches bends between small Slots
******               for realizing simplest geometries.
******             - used after beamline is constructed using
******               drifts only.
******             - supersedes global function DriftsToSlots,
******               but with an altered algorithm.
******               - recall: DriftsToSlots replaced entire drifts
******                 with Slots.
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
******  Copyright (c) 2010  Universities Research Association, Inc. / Fermilab
******                All Rights Reserved
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
******  ----------------
******  REVISION HISTORY
******  ----------------
******  January, 2010          michelotti@fnal.gov
******  - original version
******    : altered algorithm starting from DriftsToSlots.cc
******    : requires further testing and improvement but
******      am uploading nonetheless.
******
**************************************************************************
*************************************************************************/

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
#include <beamline/drift.h>
#include <beamline/Slot.h>
#include <beamline/Alignment.h>

#include <physics_toolkit/DriftConverter.h>

using namespace std;

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

DriftConverter::DriftConverter()
{
  a_rb_   = false;
  a_CFrb_ = false;
  a_sb_   = false;
  a_CFsb_ = false;
  c_rb_   = false;
  c_CFrb_ = false;
  c_sb_   = false;
  c_CFsb_ = false;
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

DriftConverter::~DriftConverter()
{
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

bool DriftConverter::lookUpStream_( beamline::iterator bi )
{
  a_ = *(--bi);

  // ??? Compare to lookDownStream_ ???
  // ??? FIX ME ???

  while( 0 != strcmp( a_->Type(), "rbend"    )  &&
         0 != strcmp( a_->Type(), "CF_rbend" )  &&
         0 != strcmp( a_->Type(), "sbend"    )  &&
         0 != strcmp( a_->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", a_->Type() ) ) {
      return false;
    }

    if  (bi == bi.begin() ) { 
      a_ = ElmPtr();
      return false;
    }

    a_ = *(--bi); 
  }

  a_rb_   = ( 0 == strcmp( a_->Type(), "rbend"    ) );
  a_CFrb_ = ( 0 == strcmp( a_->Type(), "CF_rbend" ) );
  a_sb_   = ( 0 == strcmp( a_->Type(), "sbend"    ) );
  a_CFsb_ = ( 0 == strcmp( a_->Type(), "CF_sbend" ) );

  if( !(a_rb_ || a_CFrb_ || a_sb_ || a_CFsb_ ) ) 
  {

    throw( GenericException( __FILE__, __LINE__, 
           "beamline* DriftsToSlots( beamline& original )", 
           "A horrible, inexplicable error has"
           "  occurred while looking upstream." ) );


  }

  return true;
}



/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

bool DriftConverter::lookDownStream_( beamline::iterator bi )
{
  ++bi;

  c_ = ( bi != bi.end() ) ? (*bi) : *(bi.begin());

  while( 0 != strcmp( c_->Type(), "rbend"    )  &&
         0 != strcmp( c_->Type(), "CF_rbend" )  &&
         0 != strcmp( c_->Type(), "sbend"    )  &&
         0 != strcmp( c_->Type(), "CF_sbend" )     ) 
  {
    if( 0 == strcmp( "drift", c_->Type() ) ) {
      c_ = ElmPtr();
      return false;
    }

    ++bi;

    c_ = ( bi != bi.end() ) ? (*bi) : *(bi.begin());
  }

  c_rb_   = ( 0 == strcmp( c_->Type(), "rbend"    ) );
  c_CFrb_ = ( 0 == strcmp( c_->Type(), "CF_rbend" ) );
  c_sb_   = ( 0 == strcmp( c_->Type(), "sbend"    ) );
  c_CFsb_ = ( 0 == strcmp( c_->Type(), "CF_sbend" ) );

  if( !(c_rb_ || c_CFrb_ || c_sb_ || c_CFsb_ ) ) 
  {
    if( 0.0 != c_->Length() ) {
      throw( GenericException( __FILE__, __LINE__, 
             "beamline* DriftsToSlots( beamline& original )", 
             "A horrible, inexplicable error has"
             "  occurred while looking downstream." ) );
    }
    c_ = ElmPtr();
    return false;
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

bool DriftConverter::rbendLike_( bmlnElmnt const& x)
{
  if(0 == strcmp("rbend", x.Type())) {
    rbend const* bp = static_cast<rbend const*>( &x );
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  if(0 == strcmp("CF_rbend", x.Type())) {
    CF_rbend const* bp = static_cast<CF_rbend const*>( &x );
    return ( (0.0 == bp->getEntryFaceAngle()) && (0.0 == bp->getExitFaceAngle()) );
  }
  if(0 == strcmp("sbend", x.Type())) {
    sbend const* bp = static_cast<sbend const*>( &x );
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryFaceAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitFaceAngle()  - halfBendAngle) < 1.0e-8)    );
  }
  if(0 == strcmp("CF_sbend", x.Type())) {
    CF_sbend const* bp = dynamic_cast<CF_sbend const*>(&x);
    double halfBendAngle = bp->getBendAngle() / 2.0;
    return ( (std::abs(bp->getEntryFaceAngle() - halfBendAngle) < 1.0e-8) &&
             (std::abs(bp->getExitFaceAngle()  - halfBendAngle) < 1.0e-8)      );
  }
  return false;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

bool DriftConverter::sbendLike_( bmlnElmnt const& x )
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


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


void DriftConverter::appendSlotUpstream_( BmlPtr& ret, double slotLength )
{
  double entryAngle;
  if( c_sb_ ) {
    entryAngle = boost::static_pointer_cast<sbend>(c_)->getEntryAngle();
  }
  else if( c_CFsb_ ) {
    entryAngle = boost::static_pointer_cast<CF_sbend>(c_)->getEntryAngle();
  }
  else if( c_rb_ ) {
    entryAngle =  boost::static_pointer_cast<rbend>(c_)->getEntryAngle();
  }
  else if( c_CFrb_ ) {
    entryAngle =  boost::static_pointer_cast<CF_rbend>(c_)->getEntryAngle();
  }
  else {
    ostringstream uic;
    uic  << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
         << "\n*** ERROR *** Impossible! This should never have happened."
         << "\n*** ERROR *** Upstream element is not identified as any kind of bend."
         << "\n*** ERROR *** ";
    throw( GenericException( __FILE__, __LINE__, 
           "void DriftConverter::appendSlotUpstream_( ... )", 
           uic.str().c_str() ) );
  }

  double rollAngle = c_->Alignment().tilt;

  Frame arcFrame;
  arcFrame.reset();   // Not really necessary here.

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

  Vector fd(3);
  enum { x = 0, y, z };
  fd(x) = 0.0;
  fd(y) = 0.0;
  fd(z) = slotLength;
  arcFrame.translate(fd);

  std::string name( c_->Name() + std::string( "_inSlot" ) );
  ret->append( ElmPtr( new Slot( name.c_str(), arcFrame) ) );
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


void DriftConverter::appendSlotDownstream_( BmlPtr& ret, double slotLength )
{
  double exitAngle;
  if( a_sb_ ) {
    exitAngle = boost::static_pointer_cast<sbend>(a_)->getExitAngle();
  }
  else if( a_CFsb_ ) {
    exitAngle = boost::static_pointer_cast<CF_sbend>(a_)->getExitAngle();
  }
  else if( a_rb_ ) {
    exitAngle = boost::static_pointer_cast<rbend>(a_)->getExitAngle();
  }
  else if( a_CFrb_ ) {
    exitAngle = boost::static_pointer_cast<CF_rbend>(a_)->getExitAngle();
  }
  else {
    ostringstream uic;
    uic  << "\n*** ERROR *** "
         << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
         << "\n*** ERROR *** Impossible! This should never have happened."
         << "\n*** ERROR *** Downstream element is not identified as any kind of bend."
         << "\n*** ERROR *** ";
    throw( GenericException( __FILE__, __LINE__, 
           "void DriftConverter::appendSlotDownstream_( ... )", 
           uic.str().c_str() ) );
  }

  double rollAngle = a_->Alignment().tilt;

  Frame arcFrame;
  arcFrame.reset();   // Not really necessary here.

  Vector fd(3);
  enum { x = 0, y, z };
  fd(x) = 0.0;
  fd(y) = 0.0;
  fd(z) = slotLength;
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

  std::string name( a_->Name() + std::string( "_outSlot" ) );
  ret->append( ElmPtr( new Slot( name.c_str(), arcFrame) ) );
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


BmlPtr DriftConverter::convert( beamline const& argbml )
{
  beamline original(argbml);
  // ??? FIX ME ???
  // ??? Does not use BmlPtr ???  Problem ???
  // ??? Why is it necessary to use "original" 

  // Preliminary tests for a valid, sane argument.
  // ------------------------------------------------

  const bool originalRing = ( beamline::ring == original.getLineMode() );

  // There should be more than one element
  if( original.countHowManyDeeply() < 3 ) {
    ostringstream uic;
    uic  << "\n*** ERROR *** "
            "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
            "\n*** ERROR *** There is less than three elements in beamline "
         << original.Name()
         << "\n*** ERROR *** ";
    throw( GenericException( __FILE__, __LINE__, 
           "BmlPtr DriftConverter::convert( beamline const& )", 
           uic.str().c_str() ) );
  }

  // Bends with parallel faces should not be adjacent.
  // ------------------------------------------------

  bool prev_is_rbend_like = false; // true if the previous element is rbend-like,
                                   // initialized to false

  for ( beamline::deep_iterator dit = original.deep_begin(); 
                                dit != original.deep_end(); 
                              ++dit )
  {
    ElmPtr q = *dit;
 
    if( 0.0 != q->Length() ) 
    {
      if( rbendLike_( *q ) ) 
      {
        if( prev_is_rbend_like ) {
          ostringstream uic;
          uic  << "\n*** ERROR *** "
               << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                  "\n*** ERROR *** Two rbend-like bends should neither be adjacent"
                  "\n*** ERROR *** nor separated by a thin element."
                  "\n*** ERROR *** Error occurs at" << q->Type() << " " << q->Name()
               << "\n*** ERROR *** ";
          throw( GenericException( __FILE__, __LINE__, 
                 "BmlPtr DriftConverter::convert( beamline const& )", 
                 uic.str().c_str() ) );
        } 
        else {
          prev_is_rbend_like  = true;
        }
      } 
      else 
      {
        prev_is_rbend_like  = false;
      }
    }
  }  


  // If we come out of the above loop with  prev_is_rbend_like = true, this means that the last element
  // was rbend like. In that case, if we are dealing with a ring, we need to test that the "next" (i.e. first) 
  // element, is also not  rbend like ...
  //----------------------------------------------------------------------------------------------------------- 

  if( prev_is_rbend_like && originalRing ) 
  {
    ElmPtr q = *original.deep_begin();

    if( rbendLike_(*q) ) 
    {
      ostringstream uic;
      uic  << "\n*** ERROR *** "
           << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
              "\n*** ERROR *** Two rbend-like bends should neither be adjacent"
              "\n*** ERROR *** nor separated by a thin element."
              "\n*** ERROR *** Error occurs at " << q->Type() << " " << q->Name()
           << "\n*** ERROR *** ";
      throw( GenericException( __FILE__, __LINE__, 
             "BmlPtr DriftConverter::convert( beamline const& )", 
             uic.str().c_str() ) );
    }
  }


  // Check if Slots are already present ...
  //-----------------------------------------

  for ( beamline::deep_iterator dit = original.deep_begin(); 
                                dit != original.deep_end(); 
                              ++dit )
  {
    if( 0 == strcmp( (*dit)->Type(), "Slot" ) ) {
        ostringstream uic;
        uic  << "\n*** ERROR *** "
             << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
                "\n*** ERROR *** Slots already exist in this beamline."
                "\n*** ERROR *** ";
        throw( GenericException( __FILE__, __LINE__, 
               "BmlPtr DriftConverter::convert( beamline const& )", 
               uic.str().c_str() ) );
    }
  }



  // Tests passed. Proceed ...
  // -------------------------

  ElmPtr    elPtr;
  BmlPtr    ret( new beamline );
  beamline  flatRing = original.flatten();
  // ??? FIX ME ???
  // ??? Check this for memory leak ???

  bool isUpStream, isDownStream;
  

  // Process the flattened beamline ...
  // ------------------------------  

  beamline::iterator it2;
  bool isRing = ( beamline::ring == flatRing.getLineMode() );

  for ( beamline::iterator it = flatRing.begin(); 
                           it != flatRing.end(); 
                         ++it                     )
  {
    elPtr = *it;

    bool isFirstPtr = (      it   == flatRing.begin() );
    it2 = it;
    bool isLastPtr  = (  (++it2)  == flatRing.end()   );

    // If the trial element is not a drift, just copy it ...
    // -----------------------------------------------------

    if( 0 !=  strcmp( elPtr->Type(), "drift" ) ) 
    {
      ret->append( ElmPtr( elPtr->Clone() ) );
    }

    // ... otherwise, begin processing ...
    // -----------------------------------

    else 
    {
      // Search upstream and downstream for the closest rbend, sbend, 
      // CF_rbend, or CF_sbend
      // ------------------------------------------------------------

      a_ = ElmPtr();   // Will be assigned by one of the
      c_ = ElmPtr();   // look[Up|Down]Stream_ methods, if appropriate.
      try {
        if( isRing ) {
          isDownStream  = lookUpStream_(it);
          isUpStream    = lookDownStream_(it);
        }
        else {
          if( isFirstPtr ) {
            isDownStream  = false;
            isUpStream    = lookDownStream_(it);
          }
          else if( isLastPtr ) {
            isDownStream  = lookUpStream_(it);
            isUpStream    = false;
          }
          else {
            isDownStream  = lookUpStream_(it);
            isUpStream    = lookDownStream_(it);
          }
        }
      }
      catch( const std::exception& ge ) {
        throw( GenericException( __FILE__, __LINE__, 
               "BmlPtr DriftConverter::convert( beamline const& )", 
               ge.what() ) );
      }


      // Check viability of continuing the calculation
      // ---------------------------------------------

      if( isDownStream ) {
        if( ( a_rb_ || a_CFrb_ ) && !rbendLike_( *a_ ) ) {
          ostringstream uic;
          uic  << "\n*** ERROR *** "
               << "\n*** ERROR: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** ERROR: *** Calculation discontinued."
               << "\n*** ERROR: *** Assumptions regarding rbends violated."
               << "\n*** ERROR: *** ";
          throw( GenericException( __FILE__, __LINE__, 
                 "BmlPtr DriftConverter::convert( beamline const& )", 
                 uic.str().c_str() ) );
        }
      }
      if( isUpStream ) {
        if( ( c_rb_ || c_CFrb_ ) && !rbendLike_( *c_ ) ) {
          ostringstream uic;
          uic  << "\n*** ERROR *** "
               << "\n*** ERROR: *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** ERROR: *** Calculation discontinued."
               << "\n*** ERROR: *** Assumptions regarding rbends violated."
               << "\n*** ERROR: *** ";
          throw( GenericException( __FILE__, __LINE__, 
                 "BmlPtr DriftConverter::convert( beamline const& )", 
                 uic.str().c_str() ) );
        }
      }


      // There are four possibilities
      // ----------------------------

      if( !isUpStream && !isDownStream )             // Possibility No. 1
      {
        ret->append( ElmPtr(elPtr->Clone()) );
      }
  
      else if( isUpStream && !isDownStream )         // Possibility No. 2
      {
        if( sbendLike_( *c_ ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          double sss = 0.99*Slot::minSplitLength;
          // To make true: sss < Slot::minSplitLength
          if( 0.4*(elPtr->Length()) < sss ) {
            sss = 0.4*elPtr->Length();            
          }
          double lll = elPtr->Length() - sss;

          std::string name( elPtr->Name() + std::string("_reduced") );
          ret->append( ElmPtr( new drift( name.c_str(), lll ) ) );

          appendSlotUpstream_( ret, sss );
        }
      }
  
      else if( !isUpStream && isDownStream )         // Possibility No. 3
      {
        if( sbendLike_( *a_ ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        else {
          double sss = 0.99*Slot::minSplitLength;
          // To make true: sss < Slot::minSplitLength
          if( 0.4*(elPtr->Length()) < sss ) {
            sss = 0.4*elPtr->Length();            
          }
          double lll = elPtr->Length() - sss;

          appendSlotDownstream_( ret, sss );

          std::string name( elPtr->Name() + std::string("_reduced") );
          ret->append( ElmPtr( new drift( name.c_str(), lll ) ) );
        }
      }
  
      else // isUpstream and isDownStream            // Possibility No. 4
      {
        if( sbendLike_( *a_ ) && sbendLike_( *c_ ) ) {
          ret->append( ElmPtr( elPtr->Clone() ) );
        }
        #if 0  // A restrictive alternative
        else if( rbendLike_( *a_ ) && rbendLike_( *c_ ) ) {
        #endif
        #if 1  // A restrictive alternative, opposed
        else {
        #endif
          double sss = 0.99*Slot::minSplitLength;
          // To make true: sss < Slot::minSplitLength
          if( 0.2*(elPtr->Length()) < sss ) {
            sss = 0.2*elPtr->Length();            
          }
          double lll = elPtr->Length() - 2.0*sss;

          appendSlotDownstream_( ret, sss );

          std::string name( elPtr->Name() + std::string("_reduced") );
          ret->append( ElmPtr( new drift( name.c_str(), lll ) ) );

          appendSlotUpstream_( ret, sss );
        }
        #if 0  // A restrictive alternative
        else {
          ostringstream uic;
          uic  << "\n*** ERROR *** "
               << "\n*** ERROR *** File: " << " " << __FILE__ << ", line " << __LINE__ << ": "
               << "\n*** ERROR *** Currently, conversion is supported only when"
               << "\n*** ERROR *** neighboring bends have similar type."
               << "\n*** ERROR *** ";
          throw( GenericException( __FILE__, __LINE__, 
                 "BmlPtr DriftConverter::convert( beamline const& argbml )",
                 uic.str().c_str() ) );
        }
        #endif
      }
  
    } // End processing element
  
  } // End "for" loop
  
 
  // Final adjustments
  // -----------------

  ret->setEnergy( original.Energy() );
  ret->rename( original.Name() );
  ret->setLineMode( original.getLineMode() );

  return ret;
}
