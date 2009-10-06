/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      BmlnElmnt.cc
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
****** REVISION HISTORY
******
****** Jan 2009           ostiguy@fnal.gov
****** - eliminated class alignmentData
****** - support for small pitch angles.
****** Nov 2008           ostiguy@fnal.gov
****** - added misc virtual boolean queries. These are introduced
******   to eliminate code that attempts similar queries via RTTI
******   and if-then-else blocks. 
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator
****** - added explicit implementation for assigment operator.
****** - eliminated obsolete attributes (IToField_, etc ) 
****** - setLength(), setStrength() no longer virtual
****** Apr 2008           michelotti@fnal.gov
****** - forbade negative length elements
****** Dec 2007           ostiguy@fnal.gov
****** - major refactoring, new typesafe propagator architecture
****** Jul 2007           ostiguy@fnal.gov
****** - new, less memory-hungry PinnedFrameSet implementation
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - modified visitor to reduce src file coupling. 
******   visit() now takes advantage of (reference) dynamic type.
****** - use std::string consistently for string operations. 
*****
**************************************************************************
*************************************************************************/

#include <typeinfo>
#include <string>

#include <iomanip>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/BmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>
#include <beamline/ApertureDecorator.h>
#include <beamline/AlignmentDecorator.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

 int const BMLN_dynDim = PhaseSpaceIndexing::BMLN_dynDim;

 typedef PhaseSpaceIndexing::index index;

 index const i_x   = PhaseSpaceIndexing::i_x;
 index const i_y   = PhaseSpaceIndexing::i_y;
 index const i_cdt = PhaseSpaceIndexing::i_cdt;
 index const i_npx = PhaseSpaceIndexing::i_npx;
 index const i_npy = PhaseSpaceIndexing::i_npy;
 index const i_ndp = PhaseSpaceIndexing::i_ndp;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double PropagatorTraits<Particle>::norm(  PropagatorTraits<Particle>::Component_t const& comp)
{ 
  return std::abs(comp); 
}   


double PropagatorTraits<JetParticle>::norm(  PropagatorTraits<JetParticle>::Component_t const& comp)
{ 
  return std::abs( comp.standardPart() ); 
}    

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// ***********************************
//   class BmlnElmnt::PinnedFrameSet
// ***********************************


BmlnElmnt::PinnedFrameSet::PinnedFrameSet()
:   upStream_(0), downStream_(0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt::PinnedFrameSet::PinnedFrameSet( BmlnElmnt::PinnedFrameSet const& o)
:   upStream_(0), downStream_(0)
{

  upStream_   =  o.upStream_   ?   new Frame( *(o.upStream_)   ) : 0; 
  downStream_ =  o.downStream_ ?   new Frame( *(o.downStream_) ) : 0;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt::PinnedFrameSet::~PinnedFrameSet()
{

  if (    upStream_ ) delete    upStream_; 
  if (  downStream_ ) delete  downStream_; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt::PinnedFrameSet& BmlnElmnt::PinnedFrameSet::operator=( BmlnElmnt::PinnedFrameSet const& rhs)
{

  if (    upStream_ ) delete    upStream_; 
  if (  downStream_ ) delete  downStream_; 

  upStream_   =  rhs.upStream_   ?   new Frame( *(rhs.upStream_)   ) : 0; 
  downStream_ =  rhs.downStream_ ?   new Frame( *(rhs.downStream_) ) : 0;

  return *this;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame const& BmlnElmnt::PinnedFrameSet::upStream() const
{
  return  upStream_ ? (*upStream_) : Frame::identityFrame(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame const& BmlnElmnt::PinnedFrameSet::downStream() const
{
  return  downStream_ ? (*downStream_) : Frame::identityFrame(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::PinnedFrameSet::upStream( Frame const& frame) 
{
  upStream_ = ( &frame == &(Frame::identityFrame())) ? 0 : new Frame(frame);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::PinnedFrameSet::downStream(Frame const& frame ) 
{
  upStream_ = ( &frame == & (Frame::identityFrame())) ? 0 : new Frame(frame);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::PinnedFrameSet::reset()
{
  if (upStream_)   { delete   upStream_;   upStream_ = 0; }
  if (downStream_) { delete downStream_; downStream_ = 0; }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::PinnedFrameSet::altered()  const
{
  return ( ( &( upStream()   ) != &( Frame::identityFrame() ) ) | 
           ( &( downStream() ) != &( Frame::identityFrame() ) ) ) ;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class BmlnElmnt
// **************************************************

BmlnElmnt::BmlnElmnt( std::string const&  n, double const& l, double const& s) 
  : ident_( n ),      
    length_(l),     
    strength_(s),  
    pscale_(1.0),  
    align_(0),   
    pinnedFrames_(),
    attributes_(),
    tag_(), 
    usedge_(false),
    dsedge_(false)
{
  if( length_ < 0 ) {
    ostringstream uic;
    uic  << "Argument list "
         "( " << n << ", " << l << ", " << s << " )"
         " specifies a negative length.";
    throw(  GenericException( __FILE__, __LINE__, 
           "BmlnElmnt::BmlnElmnt( const char*  n, double const& l, double const& s)", 
           uic.str().c_str() ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt::BmlnElmnt( BmlnElmnt const& o ) 
  :                  ident_(o.ident_),      
                    length_(o.length_),     
                  strength_(o.strength_),  
                    pscale_(o.pscale_),  
                     align_(0),   
              pinnedFrames_(o.pinnedFrames_),
                attributes_(o.attributes_),
                       tag_(o.tag_),
                    usedge_(o.usedge_),
                    dsedge_(o.dsedge_)
{                  
     align_ = o.align_  ? new Alignment(*o.align_)  : 0;

     propagator_ =  PropagatorPtr( o.propagator_->clone() );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt& BmlnElmnt::operator=( BmlnElmnt const& rhs )  
{
    if ( &rhs == this ) return *this;

    ident_        = rhs.ident_;      
    length_       = rhs.length_;     
    strength_     = rhs.strength_;  
    pscale_       = rhs.pscale_;  
    align_        = rhs.align_ ? new Alignment(*rhs.align_) : 0;  
    pinnedFrames_ = rhs.pinnedFrames_;
    attributes_   = rhs.attributes_;
    tag_          = rhs.tag_;
    propagator_   = PropagatorPtr(rhs.propagator_->clone());

    return *this;
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BmlnElmnt::~BmlnElmnt() 
{
  if(align_)     delete align_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::hasParallelFaces() const
{
  return  true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::hasStandardFaces() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::isBeamline() const
{                   
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::setLength( double const& length ) 
{


   if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void BmlnElmnt::setLength( double const& length )",
             "Resetting the length of a beamline is not allowed." ));
   }


   if( length < 0.0 ) {
    
    stringstream msg;  
    msg << " ***ERROR***: Lengths must be positive. You have entered length " << length 
               <<  " for " << Type() << "  " << Name() << endl;
    throw(  GenericException( __FILE__, __LINE__, 
					  "void BmlnElmnt::setLength( double const& length )", msg.str() ));
   }

   if ( isThin() ) {  
   
    stringstream msg;  
     msg << "*** ERROR ***: The length of a thin element cannot be modified. \n"
         << "You have entered length " << length 
         << " for " << Type() << "  " << Name() 
         << endl;
    throw(   GenericException( __FILE__, __LINE__, 
             "void BmlnElmnt::setLength( double const& length )", msg.str() ));
  }
 
  length_ = length;

  // Notify propagator of attribute change  

  propagator_->setAttribute(*this, "LENGTH", length );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BmlnElmnt::setStrengthScale( double const& value )
{ 
  if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void BmlnElmnt::setLength( double const& x )",
             "Resetting the strength scale of a beamline is not allowed." ) );
   }

  pscale_ = value;  

  // Notify propagator of attribute change  

  propagator_->setAttribute(*this, "STRENGTH_SCALE", value );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  BmlnElmnt::strengthScale() const
{ 
  return pscale_;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  BmlnElmnt::Strength() const
{ 
  return ( isMagnet() ? strength_*pscale_ : strength_ );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::setStrength( double const& s ) 
{

  if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void BmlnElmnt::setLength( double const& x )",
             "Setting the strength of a beamline is not allowed." ) );
   }

  strength_ = s/pscale_; 

  // Notify propagator of attribute change  
 
  propagator_->setAttribute(*this, "STRENGTH", s );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* BmlnElmnt::Name()   const
{
  return ident_.c_str();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// --- Local, small alignment routines.
// --- Changes alignment struct.
// --- Should not disturb neighboring elements.

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelX( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() ) {
    if ( !align_ ) { align_ = new Alignment(); }
    align_->setXOffset( align_->xOffset() + u );
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelY( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() ) {
    if ( !align_ ) { align_ = new Alignment(); }
    align_->setYOffset( align_->yOffset() + u );
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsX( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() ) {
    if ( !align_ ) { align_ = new Alignment(); }
    align_->setXOffset( u );
  }

  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsY( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() ) {
    if ( !align_ ) { align_ = new Alignment(); }
    align_->setYOffset( u );
  }

  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelXmm( double const& u )
{
  return  alignRelX( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelYmm( double const& u )
{
  return alignRelY( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsXmm( double const& u )
{
  return alignAbsX( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsYmm( double const& u )
{
  return alignAbsY( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelRoll( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() && hasStandardFaces() ) {

    if ( !align_) { align_ = new Alignment(); } 
    align_->setRoll( align_->roll() + u );
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double const& u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsRoll( double const& u )
{
  bool ret = true;

  if( hasParallelFaces() && hasStandardFaces() ) {
    if (!align_) { align_ = new Alignment(); } 
    align_->setRoll( u );
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool BmlnElmnt::alignRelX( double const& u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing will be done."
            "\n*** WARNING *** "
         << endl;
    ret = false;
  }
  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignRelRollmrad( double const& u )
{
  return  alignRelRoll( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::alignAbsRollmrad( double const& u )
{
  return (alignAbsRoll( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::realign()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  BmlnElmnt::realign is not finished.
  // #error *** WARNING ***

  if( align_ ) { delete align_; align_ = 0; }

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void BmlnElmnt::realign()"
                "\n*** WARNING *** --------------------------------------"
                "\n*** WARNING *** This routine is undergoing renovation "
                "\n*** WARNING *** and currently will only reset the "
                "\n*** WARNING *** attribute \"align.\""
                "\n*** WARNING *** "
                "\n*** WARNING *** Temporary workaround: clone your original "
                "\n*** WARNING *** beamline, before shifting elements around, "
                "\n*** WARNING *** and reuse it as needed."
                "\n*** WARNING *** "
             << endl; 
    firstTime = false;
  }
  #endif

  # if 0
  THE ISSUES:
    - if Slots have been introduced above and below the element, 
    they must be returned to drifts, if possible
    - if they were originally Slots, then they have to be reset too.

  pinnedFrameSet_.reset();
  #endif
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::markPins()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  BmlnElmnt::markPins is not written!!!
  // #error *** WARNING ***

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void BmlnElmnt::markPins()"
                "\n*** WARNING *** ------------------------------"
                "\n*** WARNING *** This routine is not written."
                "\n*** WARNING *** Nothing will happen."
                "\n*** WARNING *** "
             << endl; 
    firstTime = false;
  }
  #endif
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::loadPinnedCoordinates( Particle const& prtcl, Vector& ret, double pct ) const
{
  // #error *** WARNING ***
  // #error *** WARNING ***  BmlnElmnt::loadPinnedCoordinates is not tested!!
  // #error *** WARNING ***

  Vector p( Particle::PSD / 2 );
  Vector v( Particle::PSD / 2 );
  //                         |
  //                         +--------- Not foolproof.

  Vector const& state = prtcl.state();

  if( ! pinnedFrames_.altered() ) 
  {
    std::copy ( state.begin(), state.end(), ret.begin() );
    return; 
  }

  //....................................................................................

  if( Particle::PSD != ret.Dim() ) {
    (*pcerr) << "\n*** WARNING *** "
             << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
             << "\n*** WARNING *** void BmlnElmnt::loadPinnedCoordinates(...)"
             << "\n*** WARNING *** Dimension of vector argument is "
             << (ret.Dim())
             << ';'
             << "\n*** WARNING *** current implementation only allows "
             << BMLN_dynDim
             << '.'
             << "\n*** WARNING *** This function will do nothing."
             << endl;
    std::copy ( state.begin(), state.end(), ret.begin() );
    return; 
  }

  //....................................................................................

  if( pct < 0.000001 ) { pct = 0.0; }
  if( 0.999999 < pct ) { pct = 1.0; }

  // Load position and momentum vectors

  p[i_x  ] = state[i_x];
  p[i_y  ] = state[i_y];   
  p[i_cdt] = 0.0;

  v[i_x  ] = state[i_npx];  
  v[i_y  ] = state[i_npy];  
  v[i_cdt] = prtcl.npz();

  if( (1.0 - pct) < 0.001 ) {  // *** Downstream end (default)
    pinnedFrames_.downStream().convertInPlace( p, v );
  }
  else if( pct < 0.001 ) {    //  *** Upstream end
    pinnedFrames_.upStream().convertInPlace( p, v );
      }
  else {                      //  *** Somewhere in the middle
    Frame ref( Frame::tween( pinnedFrames_.upStream(), pinnedFrames_.downStream(), pct, false ) );
    ref.convertInPlace( p, v );

  }

  // Transfer and return the answer

  ret[i_x  ] = p[i_x];
  ret[i_y  ] = p[i_y];
  ret[i_cdt] = prtcl.cdt();
  ret[i_npx] = v[i_x];
  ret[i_npy] = v[i_y];
  ret[i_ndp] = prtcl.ndp();

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::setAlignment(Alignment const& a) 
{
  realign();

  if (!align_) align_ = new Alignment(); //  this is necessary because realign() deletes the alignment object ...   
                                         //  realign() remains an unfinished kludge ! 

  if( hasParallelFaces() ) {
    (*align_) = a;
    return true;
  }

  // rotation by 0 pi or pi/2 is OK even when faces are not parallel
  
  if(      ( std::abs(a.roll())                      < 1.0e-12 )
        || ( std::abs( M_PI   - std::abs(a.roll()) ) < 1.0e-9  )
        || ( std::abs( M_PI_2 - std::abs(a.roll()) ) < 1.0e-9  ) ) {
      if( (a.xOffset() == 0.0) || (a.yOffset() == 0.0) ) {
        (*align_) =  a;
        return true; 
   }

  // faces are not parallel and rotation not a multiple of pi/2 

  (*pcerr) << "\n*** WARNING *** "
            << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
            << "\n*** WARNING *** bool BmlnElmnt::setAlignment(const alignmentData& a)"
            "\n*** WARNING *** Cannot use this method on an element "
            << Type() << "  " << Name()
            << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing has been done."
            "\n*** WARNING *** "
            << endl;

    return false;
  }

  // pinnedFrames_._altered is not changed

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment BmlnElmnt::alignment() const 
{
  return align_ ? (*align_) : Alignment();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


double BmlnElmnt::getReferenceTime() const
{
  return propagator_->getReferenceTime();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::setReferenceTime( double ct )
{
  propagator_->setReferenceTime( ct );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::rename( std::string const&  n ) 
{
  ident_ = (!n.empty()) ? n : string("NONAME"); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::enableEdges(bool usedge, bool dsedge)
{
   usedge_= usedge;
   dsedge_= dsedge;
   propagator_->ctor(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::hasUpstreamEdge() const
{
  return   usedge_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool BmlnElmnt::hasDownstreamEdge() const
{
  return   dsedge_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr>  BmlnElmnt::split( double const& pc ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "BmlnElmnt::split( double const& pc )", 
           uic.str().c_str() ) );
  }

  //-----------------------------------------------------------------------------

  ElmPtr uelm( clone() );
  uelm->enableEdges(true, false);

  ElmPtr delm( clone() );
  delm->enableEdges(false, true);

  //-----------------------------------------------------------------------------
  //  strength_  is changed only when it represents the  
  //  *integrated* strength; this is the case for _thin_ elements.
  //-----------------------------------------------------------------------------

  uelm->strength_ = uelm->isThin() ? pc*strength_            : strength_;
  delm->strength_ = delm->isThin() ? ( 1.0 - pc )*strength_  : strength_;

  uelm->length_   = uelm->isThin() ?  0.0 : pc*length_;
  delm->length_   = delm->isThin() ?  0.0 : ( 1.0 - pc )*length_;

  // set the alignment struct
  // this is a STOPGAP MEASURE ... until misalignments are handled differently. 

  uelm->setAlignment( *align_ );
  delm->setAlignment( *align_ );

  // Rename

  uelm->rename( ident_ + string("_1") );
  delm->rename( ident_ + string("_2") );

  return std::make_pair(uelm, delm);


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


ostream& operator<<(ostream& os, BmlnElmnt& b)
{
#if 0
  if ( &b ) {
    os << OSTREAM_DOUBLE_PREC 
       << b.Type() 
       << " " 
       << b.Name() 
       << " " 
       << OSTREAM_DOUBLE_PREC << b.Length() 
       << " " 
       << OSTREAM_DOUBLE_PREC << b.Strength() 
       << " "
       << OSTREAM_DOUBLE_PREC << b.getReferenceTime()
       << " " ;
    os << (*b.align_) << "\n";
    b.writeTo(os); // Polymorphically call the appropriate writeTo().
  }
#endif
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BmlnElmnt::Length() const
{
  return isThin() ? 0.0: length_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::any& BmlnElmnt::operator[]( std::string const& s) 
{
  return attributes_[s.c_str()]; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  BmlnElmnt::queryAttribute( std::string const& s ) const
{
  return (attributes_.end() !=  attributes_.find( s.c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BmlnElmnt::removeAttribute( std::string const& s ) 
{
  attributes_.erase( s.c_str() ); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BmlnElmnt::removeAttribute()
{
  attributes_.clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::propagateReference( Particle& particle, double initialBRho, bool scaling ) 
{               

  double brho = particle.refBrho();
  setReferenceTime(0.0);
  particle.cdt(0.0);
 
  if ( (initialBRho != brho ) &&  isMagnet() && scaling ) { 
    setStrengthScale( brho / initialBRho ); 
  }

  propagate( particle );
  setReferenceTime( particle.cdt() );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::propagate( Particle& x ) const
{
  if( !align_  ) {
    localPropagate  ( x );
  }
  else {
    enterLocalFrame ( x );
    localPropagate  ( x );
    leaveLocalFrame ( x );
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::propagate( JetParticle& x ) const
{

  if( !align_  ) {
    localPropagate  ( x );
  }
  else {
    enterLocalFrame ( x );
    localPropagate  ( x );
    leaveLocalFrame ( x );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BmlnElmnt::propagate( ParticleBunch& x ) const
{

  if( !align_  ) {
    localPropagate  ( x );
  }
  else {

    for ( ParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { enterLocalFrame( *it ); }
    localPropagate  ( x );
    for ( ParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { leaveLocalFrame( *it ); }

  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


void BmlnElmnt::propagate( JetParticleBunch& x ) const
{

  if( !align_  ) {
    localPropagate  ( x );
  }
  else {

    for ( JetParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { enterLocalFrame( *it ); }
    localPropagate  ( x );
    for ( JetParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { leaveLocalFrame( *it ); }

  }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::localPropagate( Particle& p ) const
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::localPropagate( JetParticle& p ) const
{
  (*propagator_)(*this, p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::localPropagate( ParticleBunch& b ) const
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::localPropagate( JetParticleBunch& b ) const
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BmlnElmnt::OrbitLength( Particle const& ) const
{ 
  return length_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<BmlnElmnt::aperture_t,double,double> BmlnElmnt::aperture() const
{ 
  return (propagator_->hasAperture() ? propagator_->aperture()   
                                     : boost::tuple<aperture_t,double,double>(infinite, 0.0, 0.0) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BmlnElmnt::setAperture( BmlnElmnt::aperture_t type, double hor, double ver) 
{

  if ( !propagator_->hasAperture() ) { 
    propagator_ = PropagatorPtr(new ApertureDecorator(propagator_) ); 
  }
  
  propagator_->setAperture( type, hor, ver);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<Vector,Vector> BmlnElmnt::getAlignment() const
{ 
  return (propagator_->hasAlignment() ? propagator_->alignment()   
                                      : boost::tuple<Vector,Vector>(Vector(3), Vector(3) ));
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  BmlnElmnt::setAlignment( Vector const& translation, Vector const& rotation)  
{
  if ( !propagator_->hasAlignment() ) { 
   propagator_ = 
       PropagatorPtr( new AlignmentDecorator( PropagatorPtr( propagator_->clone() ) ) ); 
  }
  propagator_->setAlignment( translation, rotation );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

