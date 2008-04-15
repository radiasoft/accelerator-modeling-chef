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
******
****** Jul 2007           ostiguy@fnal.gov
****** - new, less memory-hungry PinnedFrameSet implementation
******   
****** Dec 2007           ostiguy@fnal.gov
****** - new typesafe propagator architecture
******                                                        
****** Apr 2008           michelotti@fnal.gov
****** - modified bmlnElmnt::setLength
****** 
**************************************************************************
*************************************************************************/

#include <typeinfo>
#include <string>

#include <iomanip>
#include <basic_toolkit/iosetup.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Aperture.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <beamline/Alignment.h>

using namespace std;
using FNAL::pcerr;
using FNAL::pcout;

namespace {

  int const  BMLN_dynDim = 6;

  Particle::PhaseSpaceIndex const& i_x   = Particle::xIndex;
  Particle::PhaseSpaceIndex const& i_y   = Particle::yIndex;
  Particle::PhaseSpaceIndex const& i_cdt = Particle::cdtIndex;
  Particle::PhaseSpaceIndex const& i_npx = Particle::npxIndex;
  Particle::PhaseSpaceIndex const& i_npy = Particle::npyIndex;
  Particle::PhaseSpaceIndex const& i_ndp = Particle::ndpIndex;
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

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( const char*  n, double const& l, double const& s) 
try
  : ident_( n ? n: "NONAME"),      
    length_(l),
    strength_(s),
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    bml_(),      
    elm_(),    
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(), 
    pAperture_(0),
    dataHook()
{
  if( length_ < 0 ) {
    ostringstream uic;
    uic  << "Argument list "
         "( " << n << ", " << l << ", " << s << " )"
         " specifies a negative length.";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "bmlnElmnt::bmlnElmnt( const char*  n, double const& l, double const& s)", 
           uic.str().c_str() ) );
    // P.S. bmlnElmnt::GenericException will be eliminated soon.
    // -lpjm
  }
}
catch( bmlnElmnt::GenericException const& ge )
{
  // This catch block is included only out of paranoia.
  // Nothing needs to be done here.
  throw ge;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( bmlnElmnt const& a ) 
  :                  ident_(a.ident_),      
                    length_(a.length_),     
                  strength_(a.strength_),  
                     align_(0),   
                  iToField_(a.iToField_),   
              shuntCurrent_(a.shuntCurrent_), 
                       bml_(),      
                       elm_(),    
              pinnedFrames_(a.pinnedFrames_),
                     ctRef_(a.ctRef_),
                attributes_(a.attributes_),
                       tag_(a.tag_),
                 pAperture_(0),
                     dataHook()
{


 pAperture_ = a.pAperture_ ? a.pAperture_->Clone()    : 0;
     align_ = a.align_     ? new alignment(*a.align_) : 0;

     init_internals(a.bml_, a.elm_); 
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
    align_        = rhs.align_ ? new alignment(*rhs.align_) : 0;  
    iToField_     = rhs.iToField_;   
    shuntCurrent_ = rhs.shuntCurrent_; 
    bml_          = BmlPtr();      
    elm_          = ElmPtr();    
    pinnedFrames_ = rhs.pinnedFrames_;
    ctRef_        = rhs.ctRef_;
    attributes_   = rhs.attributes_;
    tag_          = rhs.tag_;
    pAperture_    = rhs.pAperture_ ? rhs.pAperture_->Clone() : 0 ; 
    dataHook      = rhs.dataHook;

    init_internals(rhs.bml_, rhs.elm_); 

    return *this;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::~bmlnElmnt() {

  dataHook.clear();

  if(align_)     delete align_;
  if(pAperture_) delete pAperture_;
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Begin: basic propagator functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


void bmlnElmnt::propagate( Particle& x ) 
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

void bmlnElmnt::propagate( JetParticle& x )
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


void bmlnElmnt::propagate( ParticleBunch& x )
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


void bmlnElmnt::propagate( JetParticleBunch& x )
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



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setLength( double const& x ) 
{
  static bool firstTime = true;

  if( length_ > 0 ) {
    double oldLength = length_;

    if( x < 0.0 ) {
      (*pcerr) << "*** WARNING *** "
                "\n*** WARNING *** bmlnElmnt::setLength"
                "\n*** WARNING *** Lengths must be positive."
                "\n*** WARNING *** You have entered"
           << x 
           <<   "\n*** WARNING *** I will use its absolute value."
                "\n*** WARNING *** "
           << endl;
      length_ = -x;
    }
    else {
      length_ = x;
    }

    // ??? I am unsure whether it is better to do this      ???
    // ??? here or to zero ctRef_ and REQUIRE another pass  ???
    // ??? with the RefRegVisitor to set it correctly.      ???
    // ??? - lpjm                                           ???
    ctRef_ *= (length_/oldLength);
  }

  else {
    if( firstTime ) {
      (*pcerr) <<   "*** WARNING *** "
                  "\n*** WARNING *** bmlnElmnt::setLength"
                  "\n*** WARNING *** Attempt made to change the length of a "
                  "\n*** WARNING *** thin (i.e. zero length) element: "
               << Type() << "  " << Name()
               << "\n*** WARNING *** No change has been made."
                  "\n*** WARNING *** "
                  "\n*** WARNING *** This warning is printed only once."
                  "\n*** WARNING *** "
           << endl;
      firstTime = false;
    }
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setStrength( double const& s ) {
  strength_ = s - getShunt()*IToField(); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setCurrent( double const& I ) {
  setStrength((I-getShunt()) * IToField());
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::peekAt( double& s, const Particle& prt ) const
{ 
 (*pcout) << setw(12) << s;

 s += const_cast<bmlnElmnt*>(this)->OrbitLength( prt );  // Kludge!!

 (*pcout) << setw(12) << s           
                  << " : " 
      << setw(10) << (int) this  
      << setw(15) << ident_       
      << setw(15) << Type()      
      << setw(12) << length_      
      << setw(12) << strength_    
      << setw(12) << shuntCurrent_
      << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::equivTo( bmlnElmnt const& x ) const 
{

  if( typeid(*this) !=  typeid(x) ) {
    return false;
  }

  double maxLength   = std::max(length_,   x.length_  );
  double maxStrength = std::max(strength_, x.strength_);

  return ( ( std::abs( length_   - x.length_   ) < 1.0e-6 * maxLength   )  &&
           ( std::abs( strength_ - x.strength_ ) < 1.0e-6 * maxStrength )     );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasParallelFaces() const
{
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::hasStandardFaces() const
{
  return true;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::isSimple() const
{
  return ( (!bml_) && ( !elm_)  );
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
      // This is stupid!
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
      // This is stupid!
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
      w.tilt += u;
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
      w.tilt = u;
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
    if(    ( std::abs(a.tilt)                      < 1.0e-12 )
        || ( std::abs( M_PI   - std::abs(a.tilt) ) < 1.0e-9  )
        || ( std::abs( M_PI_2 - std::abs(a.tilt) ) < 1.0e-9  ) )
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

  state(0) -= align_->x_offset();
  state(1) -= align_->y_offset();

  if( align_->roll() != 0.0) {
    Jet temp   = state(0) * cs + state(1) * sn;
    state(1)   = state(1) * cs - state(0) * sn;
    state(0)   = temp;

    temp       = state(3) * cs + state(4) * sn;
    state(4)   = state(4) * cs - state(3) * sn;
    state(3)   = temp;
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

void bmlnElmnt::setAperture( Aperture* pAperture_in ) 
{
    //
    // aperture = x;
    //
  pAperture_ = pAperture_in;
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

void bmlnElmnt::rename( std::string n ) {

  ident_ = (!n.empty()) ? n : string("NONAME"); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Aperture* bmlnElmnt::getAperture() {
  Aperture* app = 0;
  if(pAperture_ !=0)
    app = pAperture_->Clone();
  return app;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::Split( double const& pc, ElmPtr& a, ElmPtr& b ) const
{

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double const& pc, ElmPtr& a, ElmPtr& b )", 
           uic.str().c_str() ) );
  }

  a = ElmPtr( Clone() );
  b = ElmPtr( Clone() );

  a->ident_ = ident_ + string("_1") ;
  b->ident_ = ident_ + string("_2") ;
  
  //-----------------------------------------------------------------------------
  //  strength_  is changed only when it represents the  
  //  *integrated* strength. This is the case for thin (length_ == 0.0 elements) 
  //-----------------------------------------------------------------------------

  a->strength_ = (length_ == 0.0) ? strength_  : pc*strength_;
  b->strength_ = (length_ == 0.0) ? strength_  : ( 1.0 - pc )*strength_;

  a->length_   = pc*length_;
  b->length_   = ( 1.0 - pc )*length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setShunt(double const& a) 
{ 
  // Set the value of the shunt, creating it if necessary
  setStrength( strength_ + ( shuntCurrent_ - a ) * IToField() );
  shuntCurrent_ = a;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, bmlnElmnt& b)
{
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
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::Length() const
{
  return length_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

// Exceptions

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::GenericException::GenericException( string fileName, int lineNumber, char const* fcn, char const* msg )
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  (*pcerr) << errorString << endl;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* bmlnElmnt::GenericException::what() const throw()
{
  return errorString.c_str();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::any& bmlnElmnt::operator[]( std::string const& s) 
{
  return attributes_[s.c_str()]; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  bmlnElmnt::attributeExists( std::string const& s ) const
{
  return (attributes_.end() !=  attributes_.find( s.c_str() ) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::attributeClear( std::string const& s ) 
{
  attributes_.erase( s.c_str() ); 
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::attributeClear()
{
  attributes_.clear();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  bmlnElmnt::isBeamline() const
{
   return false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::acceptInner( BmlVisitor& v )
{
  v.setInnerFlag(true);
  v.visit(*bml_);
  v.setInnerFlag(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  bmlnElmnt::acceptInner( ConstBmlVisitor& v ) const
{
  v.setInnerFlag(true);
  v.visit(*bml_);
  v.setInnerFlag(false);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( ParticleBunch& b ) 
{

 for (  ParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    localPropagate( *it ); 
 }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::localPropagate( JetParticleBunch& b ) 
{

 for (  JetParticleBunch::iterator it = b.begin(); it != b.end(); ++it )  {  
    localPropagate( *it ); 
 }
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setReferenceTime( Particle& particle) 
{               
  setReferenceTime(0.0);
  propagate( particle );
  setReferenceTime( particle.get_cdt() );
}
