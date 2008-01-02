/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      BendEliminator.cc
******  
******             Implementation of class BendEliminator
******             - concatenates sbends and rbends removes passive
******               elements of zero length between them.
******                                                                
******  Copyright (c) 2007  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
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
****** REVISION HISTORY
******
****** October 17, 2007  Leo Michelotti
******                   michelotti@fnal.gov
****** - original version: modeled after QuadEliminator.cc
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <string>

#include <basic_toolkit/GenericException.h>
#include <physics_toolkit/BendEliminator.h>

#include <beamline/sbend.h>
#include <beamline/rbend.h>
#include <beamline/beamline.h>

using namespace std;

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BendEliminator::BendEliminator()
:   bmlPtr_()
  , relaxed_(false)
  , build_sbend_(false)
  , build_rbend_(false)
  , not_started_(true)
  , length_(0.0)
  , field_(0.0)
  , bendAngle_(0.0)
  , usFaceAngle_(0.0)
  , dsFaceAngle_(0.0)
  , new_bend_name_()
  , new_alignmentData_()
{}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BendEliminator::~BendEliminator()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool BendEliminator::getRelaxed()
{
  return relaxed_;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BendEliminator::setRelaxed( bool x )
{
  relaxed_ = x;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bool BendEliminator::toggleRelaxed()
{
  bool ret = relaxed_;
  relaxed_ = !relaxed_;
  return ret;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BendEliminator::visit( beamline const& x )
{
  // Create the new beamline
  string nuNam = x.Name() + string("_BendsMerged");
  bmlPtr_ = BmlPtr( new beamline( nuNam.c_str() ) );
  bmlPtr_->setEnergy( x.Energy() );

  // Process the argument
  for (beamline::const_deep_iterator it  = x.deep_begin();
                                     it != x.deep_end(); ++it )  {
    (*it)->accept( *this );
  }

  // Append final bend, if it exists. (IN A SANE WORLD, IT WOULD NOT!!)
  //   Note: it should be impossible for both build_sbend_
  //   and build_rbend_ to be true simultaneously.
  if( build_sbend_ ) {
    bmlPtr_->append( SbendPtr( new sbend(   new_bend_name_.c_str() 
                                          , length_
                                          , field_
                                          , bendAngle_
                                          , usFaceAngle_
                                          , dsFaceAngle_    ) ) );
    build_sbend_ = false;
  }
  else if( build_rbend_ ) {
    bmlPtr_->append( RbendPtr( new rbend(   new_bend_name_.c_str() 
                                          , length_
                                          , field_
                                          , usFaceAngle_
                                          , dsFaceAngle_    ) ) );
    build_rbend_ = false;
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BendEliminator::visit( bmlnElmnt const& x )
{
  if( bmlPtr_ )  {  // Not paranoia!

    if( build_sbend_ ) {
      bmlPtr_->append( SbendPtr( new sbend(   new_bend_name_.c_str() 
                                            , length_
                                            , field_
                                            , bendAngle_
                                            , usFaceAngle_
                                            , dsFaceAngle_    ) ) );
      build_sbend_ = false;
    }
    else if( build_rbend_ ) {
      bmlPtr_->append( RbendPtr( new rbend(   new_bend_name_.c_str() 
                                            , length_
                                            , field_
                                            , usFaceAngle_
                                            , dsFaceAngle_    ) ) );
      build_rbend_ = false;
    }

    bmlPtr_->append( ElmPtr( x.Clone() ) ); 

    if( not_started_ ) { not_started_ = false; }
  }

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BendEliminator::visit( sbend const& x )
{
  // ----------------------------------------------------------
  // It is possible, in principle, for someone to have
  //   split a bend in such a way that its ds piece
  //   begins a beamline, and its upstream piece ends it
  //   - OR -
  //   the upstream piece could be in a totally different line.
  // 
  // Handling that is prevented here. At least one non-bend element
  //   must begin the line.  This solution is not sufficient
  //   and presumably will be improved about the same time that
  //   hell freezes over.
  // ----------------------------------------------------------
  if( not_started_ ) {
    ostringstream uic;
    uic  <<   "This version of BendEliminator does not permit"
            "\nthe top-level line to begin with a dipole."
            "\nLine " << bmlPtr_->Name()
         << " begins with " << x.Type() << "  " << x.Name();
    throw( GenericException( __FILE__, __LINE__, 
           "void BendEliminator::visit( sbend const& x )", 
           uic.str().c_str() ) );
  }

  // ----------------------------------------------------------
  // It is possible, in principle, for someone 
  //   to have placed an rbend and sbend together.
  // 
  // Handling that is prevented here.
  // ----------------------------------------------------------
  if( build_rbend_ ) {
    ostringstream uic;
    uic  <<   "No version of BendEliminator wille ever permit"
            "\nan rbend to immediately precede an sbend."
            "\nFault detected at " << x.Type() << "  " << x.Name();
    throw( GenericException( __FILE__, __LINE__, 
           "void BendEliminator::visit( sbend const& x )", 
           uic.str().c_str() ) );
  }

  // ------------------------------------------------------------------------
  // We may proceed with caution
  // ... which is not to say I've done this right.
  // ------------------------------------------------------------------------
  if( !build_sbend_ ) {
    length_              = x.Length();
    field_               = x.Strength();
    bendAngle_           = x.getBendAngle();
    usFaceAngle_   = x.getEntryFaceAngle();
    dsFaceAngle_ = x.getExitFaceAngle();
    new_bend_name_       = x.Name();
    new_alignmentData_   = x.Alignment();
    build_sbend_         = true;
    // NOTE: if a personal propagator (ProfFunc*) has been
    //   written, it is NOT handled.  There is no way
    //   of assuring that these would be merged correctly.
  }
  else if(    ( new_alignmentData_  == x.Alignment() )
           && ( field_              == x.Strength()  ) ) {

    bool accepted = false;
    if( relaxed_ ) {
      accepted = std::abs( dsFaceAngle_ + x.getEntryFaceAngle() ) < 1.0e-9;
    }
    else {
      accepted =    ( 0. == dsFaceAngle_  )
                 && ( 0. == x.getEntryFaceAngle() );
    }
    if(accepted) {
      length_             += x.Length();
      bendAngle_          += x.getBendAngle();
      dsFaceAngle_ = x.getExitFaceAngle();
      // NOTE: if a personal propagator (ProfFunc*) has been
      //   written, it is NOT handled.  There is no way
      //   of assuring that these would be merged correctly.

      new_bend_name_ = new_bend_name_ + string("+") +  x.Name();
     }
    else {
      ostringstream uic;
      uic  << "Sbend exit and entry angles not matched."
           << "\n downstream: entry angle = " << x.getEntryFaceAngle()
           << " for " << x.Type() << "  " << x.Name();
      throw( GenericException( __FILE__, __LINE__, 
             "void BendEliminator::visit( sbend const& x )", 
             uic.str().c_str() ) );
    }
  }
  else {
    ostringstream uic;
    uic  <<   "Cannot merge because magnetic field"
            "\nor alignment is not matched."
            "\nFault detected at " << x.Type() << "  " << x.Name();
    throw( GenericException( __FILE__, __LINE__,
           "void BendEliminator::visit( sbend const& x )",
           uic.str().c_str() ) );
  }
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BendEliminator::visit( rbend const& x )
{
  // !!!                !!!
  // !!! TO BE FINISHED !!!
  // !!!                !!!
  ostringstream uic;
  uic  << "Sorry. this method is not written yet."
       << "\nSend a complaint to michelotti@fnal.gov";
  throw( GenericException( __FILE__, __LINE__, 
         "void BendEliminator::visit( rbend const& x )",
         uic.str().c_str() ) );
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlPtr BendEliminator::beamlinePtr()
{
  return bmlPtr_;
  // Invoking routine is responsible for
  // using this beamline before the 
  // BendEliminator goes out of scope.
}
