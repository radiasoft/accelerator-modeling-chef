/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      bmlnElmnt.cc
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
****** Apr 2008           michelotti@fnal.gov
****** - forbade negative length elements
****** 
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - modified visitor to reduce src file coupling. 
******   visit() now takes advantage of (reference) dynamic type.
****** - use std::string consistently for string operations. 
****** Jul 2007           ostiguy@fnal.gov
****** - new, less memory-hungry PinnedFrameSet implementation
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
****** Apr 2008           michelotti@fnal.gov
****** - modified bmlnElmnt::setLength
****** May 2008           ostiguy@fnal.gov
****** - attribute changes now dispatched to propagator
****** - added explicit implementation for assigment operator.
****** - eliminated obsolete attributes (IToField_, etc ) 
****** - setLength(), setStrength() no longer virtual
**************************************************************************
*************************************************************************/

#include <typeinfo>
#include <string>

#include <iomanip>
#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/GenericException.h>
#include <beamline/bmlnElmnt.h>
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

  int const  BMLN_dynDim = 6;

  Particle::PhaseSpaceIndex const& i_x   = Particle::i_x;
  Particle::PhaseSpaceIndex const& i_y   = Particle::i_y;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::i_cdt;
  Particle::PhaseSpaceIndex const& i_npx = Particle::i_npx;
  Particle::PhaseSpaceIndex const& i_npy = Particle::i_npy;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::i_ndp;
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
//   class bmlnElmnt::PinnedFrameSet
// ***********************************


bmlnElmnt::PinnedFrameSet::PinnedFrameSet()
:   upStream_(0), downStream_(0)
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::PinnedFrameSet::PinnedFrameSet( bmlnElmnt::PinnedFrameSet const& o)
:   upStream_(0), downStream_(0)
{

  upStream_   =  o.upStream_   ?   new Frame( *(o.upStream_)   ) : 0; 
  downStream_ =  o.downStream_ ?   new Frame( *(o.downStream_) ) : 0;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::PinnedFrameSet::~PinnedFrameSet()
{

  if (    upStream_ ) delete    upStream_; 
  if (  downStream_ ) delete  downStream_; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::PinnedFrameSet& bmlnElmnt::PinnedFrameSet::operator=( bmlnElmnt::PinnedFrameSet const& rhs)
{

  if (    upStream_ ) delete    upStream_; 
  if (  downStream_ ) delete  downStream_; 

  upStream_   =  rhs.upStream_   ?   new Frame( *(rhs.upStream_)   ) : 0; 
  downStream_ =  rhs.downStream_ ?   new Frame( *(rhs.downStream_) ) : 0;

  return *this;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame const& bmlnElmnt::PinnedFrameSet::upStream() const
{
  return  upStream_ ? (*upStream_) : Frame::identityFrame(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Frame const& bmlnElmnt::PinnedFrameSet::downStream() const
{
  return  downStream_ ? (*downStream_) : Frame::identityFrame(); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::PinnedFrameSet::upStream( Frame const& frame) 
{
  upStream_ = ( &frame == &(Frame::identityFrame())) ? 0 : new Frame(frame);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::PinnedFrameSet::downStream(Frame const& frame ) 
{
  upStream_ = ( &frame == & (Frame::identityFrame())) ? 0 : new Frame(frame);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::PinnedFrameSet::reset()
{
  if (upStream_)   { delete   upStream_;   upStream_ = 0; }
  if (downStream_) { delete downStream_; downStream_ = 0; }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::PinnedFrameSet::altered()  const
{
  return ( ( &( upStream()   ) != &( Frame::identityFrame() ) ) | 
           ( &( downStream() ) != &( Frame::identityFrame() ) ) ) ;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


// **************************************************
//   class bmlnElmnt
// **************************************************

bmlnElmnt::bmlnElmnt( std::string const&  n, double const& l, double const& s) 
  : ident_( n ),      
    length_(l),     
    strength_(s),  
    pscale_(1.0),  
    align_(0),   
    bml_(),      
    elm_(),    
    pinnedFrames_(),
    ctRef_(0.0),
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
           "bmlnElmnt::bmlnElmnt( const char*  n, double const& l, double const& s)", 
           uic.str().c_str() ) );
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( bmlnElmnt const& o ) 
  :                  ident_(o.ident_),      
                    length_(o.length_),     
                  strength_(o.strength_),  
                    pscale_(o.pscale_),  
                     align_(0),   
                       bml_(),      
                       elm_(),    
              pinnedFrames_(o.pinnedFrames_),
                     ctRef_(o.ctRef_),
                attributes_(o.attributes_),
                       tag_(o.tag_),
                    usedge_(o.usedge_),
                    dsedge_(o.dsedge_)
{                  

     align_ = o.align_     ? new alignment(*o.align_)  : 0;

     propagator_ =  PropagatorPtr( o.propagator_->Clone() );

     init_internals(o.bml_, o.elm_); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::init_internals( BmlPtr const& bml, ElmPtr const& elm )
{
 // ---------------------------------------------------------------------------------
 // If neither the beamline or the "element of interest" elm_ are  defined, 
 // we are done, just return.  
 //----------------------------------------------------------------------------------

 if ( (!bml ) && (!elm) ) return; 

 // ---------------------------------------------------------------------------------
 // If only the "element of interest" elm_ is defined, and the beamline bml_ is not,
 // clone the element of interest and return;
 //----------------------------------------------------------------------------------

 if( !bml ) {
    elm_ = (elm) ? ElmPtr(elm->Clone()) : ElmPtr(); 
    return;
 } 

 // -------------------------------------------------------------------------------------------------------------------------
 // If we get here, the beamline bml_ is defined. The element of interest may or may not be defined. Typically, 
 // elm_ would be defined for elements modeled with a *single* thin kick in the middle, but not for elements 
 // modeled with multiple thin kicks. 
 //---------------------------------------------------------------------------------------------------------------------------

 bml_ = BmlPtr( bml->Clone() );

 if (!elm_) return;  // there is no element of interest ... were are done 


 beamline::iterator it  =   bml_->begin(); 

 // find the position of the element of interest in the orginal beamline. The set the element of interest in the new one  
 // to the element with the same position

 for (  beamline::iterator ita  = bml->begin(); ita != bml->end(); ++ita, ++it ) {

   if( (*ita) == elm ) { elm_ = (*it);  return; }  // element of interest found. All OK. We are done.
 }
    
 //--------------------------------------------------------------------------------------------------------------------------
 // If we get here, this means that the element of interest exists, but was not found within the beamline.  
 // In that case, we simply clone the element.
 //--------------------------------------------------------------------------------------------------------------------------

 elm_ = ElmPtr( elm->Clone() );
} 

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt& bmlnElmnt::operator=( bmlnElmnt const& rhs )  
{

    if ( &rhs == this ) return *this;

    ident_        = rhs.ident_;      
    length_       = rhs.length_;     
    strength_     = rhs.strength_;  
    pscale_       = rhs.pscale_;  
    align_        = rhs.align_ ? new alignment(*rhs.align_) : 0;  
    bml_          = BmlPtr();      
    elm_          = ElmPtr();    
    pinnedFrames_ = rhs.pinnedFrames_;
    ctRef_        = rhs.ctRef_;
    attributes_   = rhs.attributes_;
    tag_          = rhs.tag_;
    propagator_   = PropagatorPtr(rhs.propagator_->Clone());

    init_internals(rhs.bml_, rhs.elm_); 

    return *this;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::~bmlnElmnt() {

  if(align_)     delete align_;

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::isSimple() const
{
  return ( !bml_ );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasParallelFaces() const
{
  return  true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasStandardFaces() const
{
  return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::isBeamline() const
{                   
  return false;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setLength( double const& length ) 
{


   if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void bmlnElmnt::setLength( double const& length )",
             "Resetting the length of a beamline is not allowed." ));
   }


   if( length < 0.0 ) {
    
    stringstream msg;  
    msg << " ***ERROR***: Lengths must be positive. You have entered length " << length 
               <<  " for " << Type() << "  " << Name() << endl;
    throw(  GenericException( __FILE__, __LINE__, 
                              "void bmlnElmnt::setLength( double const& length )", msg.str() ));
   }

   if ( isThin() ) {  
   
    stringstream msg;  
     msg << "*** ERROR ***: The length of a thin element cannot be modified. \n"
         << "You have entered length " << length 
         << " for " << Type() << "  " << Name() 
         << endl;
    throw(   GenericException( __FILE__, __LINE__, 
             "void bmlnElmnt::setLength( double const& length )", msg.str() ));
  }
 
  length_ = length;

  // Notify propagator of attribute change  

  propagator_->setAttribute(*this, "LENGTH", length );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::setStrengthScale( double const& value )
{ 
  if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void bmlnElmnt::setLength( double const& x )",
             "Resetting the strength scale of a beamline is not allowed." ) );
   }

  pscale_ = value;  

  // Notify propagator of attribute change  

  propagator_->setAttribute(*this, "STRENGTH_SCALE", value );

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  bmlnElmnt::strengthScale() const
{ 
  return pscale_;  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double  bmlnElmnt::Strength() const
{ 
  return ( isMagnet() ? strength_*pscale_ : strength_ );  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setStrength( double const& s ) 
{

  if ( isBeamline() ) {

    throw(   GenericException( __FILE__, __LINE__, 
             "void bmlnElmnt::setLength( double const& x )",
             "Resetting the strength of a beamline is not allowed." ) );
   }

  strength_ = s/pscale_; 

  // Notify propagator of attribute change  

  propagator_->setAttribute(*this, "STRENGTH", s );

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

char const* bmlnElmnt::Name()   const
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

bool bmlnElmnt::alignRelX( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() ) {
    if( !align_ ) {
      align_ = new alignment(u, 0.0, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(Alignment());
      w.xOffset += u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
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

bool bmlnElmnt::alignRelY( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() ) {
    if( !align_ ) {
      align_ = new alignment(0.0, u, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(Alignment());
      w.yOffset += u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
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

bool bmlnElmnt::alignAbsX( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() ) {
    if( !align_ ) {
      align_ = new alignment(u, 0.0, 0.0);
    }
    else {
      alignmentData w(Alignment());
      w.xOffset = u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
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

bool bmlnElmnt::alignAbsY( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() ) {
    if( !align_ ) {
      align_ = new alignment(0.0, u, 0.0);
    }
    else {
      alignmentData w(Alignment());
      w.yOffset = u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
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

bool bmlnElmnt::alignRelXmm( double const& u )
{
  return  alignRelX( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignRelYmm( double const& u )
{
  return alignRelY( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsXmm( double const& u )
{
  return alignAbsX( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsYmm( double const& u )
{
  return alignAbsY( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignRelRoll( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() && hasStandardFaces() ) {
    if( !align_ ) {
      align_ = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(Alignment());
      w.roll += u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double const& u )"
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

bool bmlnElmnt::alignAbsRoll( double const& u )
{
  bool ret = true;
  if( hasParallelFaces() && hasStandardFaces() ) {
    if( !align_ ) {
      align_ = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(Alignment());
      w.roll = u;
      setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double const& u )"
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

bool bmlnElmnt::alignRelRollmrad( double const& u )
{
  return  alignRelRoll( 0.001*u );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsRollmrad( double const& u )
{
  return (alignAbsRoll( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::realign()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::realign is not finished.
  // #error *** WARNING ***

  if( align_ ) { delete align_; align_ = 0; }

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void bmlnElmnt::realign()"
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

void bmlnElmnt::markPins()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::markPins is not written!!!
  // #error *** WARNING ***

  #if 1
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** "
                "\n*** WARNING *** File " << __FILE__ << ", Line " << __LINE__
             << "\n*** WARNING *** void bmlnElmnt::markPins()"
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

void bmlnElmnt::loadPinnedCoordinates( Particle const& prtcl, Vector& ret, double pct ) const
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::loadPinnedCoordinates is not tested!!
  // #error *** WARNING ***

  Vector p( Particle::PSD / 2 );
  Vector v( Particle::PSD / 2 );
  //                         |
  //                         +--------- Not foolproof.

  Vector const& state = prtcl.State();

  if( ! pinnedFrames_.altered() ) 
  {
    std::copy ( state.begin(), state.end(), ret.begin() );
    return; 
  }

  //....................................................................................

  if( Particle::PSD != ret.Dim() ) {
    (*pcerr) << "\n*** WARNING *** "
             << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
             << "\n*** WARNING *** void bmlnElmnt::loadPinnedCoordinates(...)"
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

  if( pct < 0.000001 ) { pct = 0; }
  if( 0.999999 < pct ) { pct = 1; }

  // Load position and momentum vectors

  p[i_x  ] = state[i_x];
  p[i_y  ] = state[i_y];   
  p[i_cdt] = 0.0;

  v[i_x  ] = state[i_npx];  
  v[i_y  ] = state[i_npy];  
  v[i_cdt] = prtcl.get_npz();

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
  ret[i_cdt] = prtcl.get_cdt();
  ret[i_npx] = v[i_x];
  ret[i_npy] = v[i_y];
  ret[i_ndp] = prtcl.get_ndp();

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::setAlignment(alignmentData const& a) 
{
  bool ret = true;
  alignment* nuAlignPtr = new alignment(a);

  realign();

  if( nuAlignPtr->isNull() ) {
    delete nuAlignPtr;
    nuAlignPtr = 0;
  }
  else if( hasParallelFaces() ) {
    align_ = nuAlignPtr;
  }
  else 
  {
    if(    ( std::abs(a.roll)                      < 1.0e-12 )
        || ( std::abs( M_PI   - std::abs(a.roll) ) < 1.0e-9  )
        || ( std::abs( M_PI_2 - std::abs(a.roll) ) < 1.0e-9  ) )
    {
      if( (a.xOffset == 0.0) || (a.yOffset == 0.0) ) {
        align_ = nuAlignPtr;
      }
      else {
        ret = false;
      }
    }
  }

  if( !ret ) {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::setAlignment(const alignmentData& a)"
            "\n*** WARNING *** Cannot use this method on an element "
         << Type() << "  " << Name()
         << "\n*** WARNING *** without affecting its neighbors."
            "\n*** WARNING *** Nothing has been done."
            "\n*** WARNING *** "
         << endl;

    if( 0 != nuAlignPtr ) { delete nuAlignPtr; nuAlignPtr = 0; }
    ret = false;
  }

  // pinnedFrames_._altered is not changed

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData bmlnElmnt::Alignment() const {

  alignmentData x;
  if( align_) {
    x = align_->getAlignment();
  }
  return x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// --- End of local, small alignment routines.

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::enterLocalFrame( Particle& p ) const
{

  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  Vector& state = p.State();

  state[0] -= align_->x_offset();
  state[1] -= align_->y_offset();

  if( align_->roll() != 0.0) {

    double temp  = state[0] * cs + state[1] * sn;
    state[1]     = state[1] * cs - state[0] * sn;
    state[0]     = temp;

    temp       = state[3] * cs + state[4] * sn;
    state[4]   = state[4] * cs - state[3] * sn;
    state[3]   = temp;
  }
 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::enterLocalFrame( JetParticle& p ) const
{

  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  JetVector& state = p.State();

  state[0] -= align_->x_offset();
  state[1] -= align_->y_offset();

  if( align_->roll() != 0.0) {
    Jet temp   = state[0] * cs + state[1] * sn;
    state[1]   = state[1] * cs - state[0] * sn;
    state[0]   = temp;

    temp       = state[3] * cs + state[4] * sn;
    state[4]   = state[4] * cs - state[3] * sn;
    state[3]   = temp;
  }


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::leaveLocalFrame( Particle& p ) const
{

  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  Vector& state = p.State();

  if( align_->roll() != 0.0) {
    double temp   = state[0] * cs - state[1] * sn;
    state[1]      = state[1] * cs + state[0] * sn;
    state[0]      = temp;

    temp        = state[3] * cs - state[4] * sn;
    state[4]    = state[4] * cs + state[3] * sn;
    state[3]    = temp;
  }

  state[0] += align_->x_offset();
  state[1] += align_->y_offset();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::leaveLocalFrame( JetParticle& p ) const
{

  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  Mapping& state = p.State();

  if( align_->roll() != 0.0) {
    Jet temp   = state[0] * cs - state[1] * sn;
    state[1]   = state[1] * cs + state[0] * sn;
    state[0]   = temp;

    temp     = state[3] * cs - state[4] * sn;
    state[4] = state[4] * cs + state[3] * sn;
    state[3] = temp;
  }

  state[0] += align_->x_offset();
  state[1] += align_->y_offset();


}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::getReferenceTime() const
{
   return ctRef_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setReferenceTime( double const& x )
{
  ctRef_ = x;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::rename( std::string const&  n ) 
{
  ident_ = (!n.empty()) ? n : string("NONAME"); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::enableEdges(bool usedge, bool dsedge)
{
   usedge_= usedge;
   dsedge_= dsedge;
   propagator_->setup(*this);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasUpstreamEdge() const
{
  return   usedge_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasDownstreamEdge() const
{
  return   dsedge_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::pair<ElmPtr,ElmPtr>  bmlnElmnt::split( double const& pc ) const
{
  // Preliminary tests ...
  // -----------------------------
  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( GenericException( __FILE__, __LINE__, 
           "bmlnElmnt::split( double const& pc )", 
           uic.str().c_str() ) );
  }

  //-----------------------------------------------------------------------------

  ElmPtr uelm( Clone() );
  uelm->enableEdges(true, false);

  ElmPtr delm( Clone() );
  delm->enableEdges(false, true);

  // ---------------------------------------------
  // Note: cloning should set the alignment struct
  // via the bmlnElmnt copy constructor.  (see above)
  // ---------------------------------------------

  //-----------------------------------------------------------------------------
  //  strength_  is changed only when it represents the  
  //  *integrated* strength; this is the case for _thin_ elements.
  //-----------------------------------------------------------------------------

  uelm->strength_ = uelm->isThin() ? pc*strength_            : strength_;
  delm->strength_ = delm->isThin() ? ( 1.0 - pc )*strength_  : strength_;

  uelm->length_   = uelm->isThin() ? 0.0 : pc*length_;
  delm->length_   = delm->isThin() ? 0.0 : ( 1.0 - pc )*length_;

  // set the alignment struct
  // this is a STOPGAP MEASURE ... until misalignments are handled differently. 

  uelm->setAlignment( Alignment() );
  delm->setAlignment( Alignment() );

  // Rename

  uelm->rename( ident_ + string("_1") );
  delm->rename( ident_ + string("_2") );

  return std::make_pair(uelm, delm);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, bmlnElmnt& b)
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

double bmlnElmnt::Length() const
{
  return isThin() ? 0.0: length_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::any& bmlnElmnt::operator[]( std::string const& s) 
{
  return attributes_[s.c_str()]; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  bmlnElmnt::queryAttribute( std::string const& s ) const
{
  return (attributes_.end() !=  attributes_.find( s.c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::removeAttribute( std::string const& s ) 
{
  attributes_.erase( s.c_str() ); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::removeAttribute()
{
  attributes_.clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::propagateReference( Particle& particle, double initialBRho, bool scaling ) 
{               

  double brho = particle.ReferenceBRho();
  setReferenceTime(0.0);
  particle.set_cdt(0.0);
 
  if ( (initialBRho != brho ) &&  isMagnet() && scaling ) { 
    setStrengthScale( brho / initialBRho ); 
  }

  propagate( particle );

  setReferenceTime( particle.get_cdt() );

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::propagate( Particle& x ) const
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

void bmlnElmnt::propagate( JetParticle& x ) const
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


void bmlnElmnt::propagate( ParticleBunch& x ) const
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


void bmlnElmnt::propagate( JetParticleBunch& x ) const
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

void bmlnElmnt::localPropagate( Particle& p ) const
{
  (*propagator_)(*this, p);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticle& p ) const
{
  (*propagator_)(*this, p);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( ParticleBunch& b ) const
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticleBunch& b ) const
{
  (*propagator_)(*this, b);
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::OrbitLength( Particle const& ) const
{ 
  return length_; 
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<bmlnElmnt::aperture_t,double,double> bmlnElmnt::aperture() const
{ 
  return (propagator_->hasAperture() ? propagator_->aperture()   
                                     : boost::tuple<aperture_t,double,double>(infinite, 0.0, 0.0) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setAperture( bmlnElmnt::aperture_t type, double hor, double ver) 
{

  if ( !propagator_->hasAperture() ) { 
    propagator_ = PropagatorPtr(new ApertureDecorator(propagator_) ); 
  }
  
  propagator_->setAperture( type, hor, ver);
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::tuple<Vector,Vector> bmlnElmnt::getAlignment() const
{ 
  return (propagator_->hasAlignment() ? propagator_->alignment()   
                                      : boost::tuple<Vector,Vector>(Vector(3), Vector(3) ));
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::setAlignment( Vector const& translation, Vector const& rotation)  
{
  if ( !propagator_->hasAlignment() ) { 
   propagator_ = 
       PropagatorPtr( new AlignmentDecorator( PropagatorPtr( propagator_->Clone() ) ) ); 
  }
  propagator_->setAlignment( translation, rotation );
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

