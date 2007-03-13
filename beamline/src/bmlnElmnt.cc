/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.2
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
****** Mar 2007           ostiguy@fnal.gov
****** - support for reference counted elements
****** - reduced src file coupling due to visitor interface. 
******   visit() takes advantage of (reference) dynamic type.
****** - use std::string for string operations. 
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



// **************************************************
//   yes and no Criteria
// **************************************************

beamline::Aye beamline::yes;
beamline::Nay beamline::no;


// ***********************************
//   class bmlnElmnt::PinnedFrameSet
// ***********************************


bmlnElmnt::PinnedFrameSet::PinnedFrameSet()
:   _altered(false)
  , _upStream()
  , _downStream()
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::PinnedFrameSet::reset()
{
  _upStream.reset();
  _downStream.reset();
  _altered = false;
}


// **************************************************
//   class bmlnElmnt
// **************************************************

bmlnElmnt::bmlnElmnt( const char* n, PropFunc* pf )
  : ident_( n ? n : "NONAME"),      
    length_(0.0),     
    strength_(0.0),  
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(pf),
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(),
    pAperture_(0)
 {}
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


bmlnElmnt::bmlnElmnt( double const& l, PropFunc* pf )
  : ident_("NONAME"),      
    length_(l),     
    strength_(0.0),  
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(pf),
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(),
    pAperture_(0),
    dataHook()
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( double const& length, double const& strength, PropFunc* pf )
  : ident_("NONAME"),      
    length_(length),     
    strength_(strength),  
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(pf),
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(),
    pAperture_(0),
    dataHook()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( const char*  n, double const& l, PropFunc* pf ) 
  : ident_( n ? n: "NONAME" ),      
    length_(l),     
    strength_(0.0),  
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(pf),
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(),
    pAperture_(0),
    dataHook()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( const char*  n, double const& l, double const& s, PropFunc* pf ) 
  : ident_( n ? n: "NONAME"),      
    length_(l),     
    strength_(s),  
    align_(0),   
    iToField_(1.0),   
    shuntCurrent_(0.0), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(pf),
    pinnedFrames_(),
    ctRef_(0.0),
    attributes_(),
    tag_(), 
    pAperture_(0),
    dataHook()
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::bmlnElmnt( bmlnElmnt const& a ) 
  : ident_(a.ident_),      
    length_(a.length_),     
    strength_(a.strength_),  
    align_(0),   
    iToField_(a.iToField_),   
    shuntCurrent_(a.shuntCurrent_), 
    p_bml_(),      
    bml_e_(),    
    propfunc_(a.propfunc_),
    pinnedFrames_(),
    ctRef_(a.ctRef_),
    attributes_(a.attributes_),
    tag_(a.tag_),
    pAperture_(0),
    dataHook()
{


 if(a.pAperture_ != 0) pAperture_ = a.pAperture_->Clone();
 
 if(a.align_ != 0) {
   alignmentData data = a.align_->getAlignment();
   if((data.xOffset != 0.0) || (data.yOffset != 0.0) || (data.tilt != 0.0))
     align_         = new alignment(*a.align_);
 } 
 else align_ = 0;

 // ---------------------------------------------------------------------------------
 // If neither the beamline or the "element of interest" bml_e_ are  defined, 
 // we are done, just return.  
 //----------------------------------------------------------------------------------

 if (( !a.p_bml_ ) && (!a.bml_e_)) return; 

 // ---------------------------------------------------------------------------------
 // If only the "element of interest" bml_e_ is defined, and the beamline p_bml_ is not,
 // clone the element of interest and return;
 //----------------------------------------------------------------------------------

 if( !a.p_bml_ ) {
    bml_e_ = (a.bml_e_) ? ElmPtr(a.bml_e_->Clone()) : ElmPtr(); 
    return;
 } 

 // -------------------------------------------------------------------------------------------------------------------------
 // If we get here, the beamline p_bml_ is defined. The element of interest may or may not be defined. Typically, 
 // bml_e_ would be defined for elements modeled with a *single* thin kick in the middle, but not for elements 
 // modeled with multiple thin kicks. 
 //---------------------------------------------------------------------------------------------------------------------------

 p_bml_ = BmlPtr( a.p_bml_->Clone() );

 if (!bml_e_) return;  // there is no element of interest ... were are done 


 beamline::iterator it  =   p_bml_->begin(); 

 // find the position of the element of interest in the orginal beamline. The set the element of interest in the new one  
 // to the element with the same position

 for (  beamline::iterator ita  = a.p_bml_->begin(); ita != a.p_bml_->end(); ++ita, ++it ) {

   if( (*ita) == a.bml_e_ ) { bml_e_ = (*it);  return; }  // element of interest found. All OK. We are done.
 }
    
 //--------------------------------------------------------------------------------------------------------------------------
 // If we get here, this means that the element of interest exists, but was not found within the beamline. This is 
 // most likely a bug. Clone the element, and continue nevertheless, and  issue a warning.   
 //--------------------------------------------------------------------------------------------------------------------------

 bml_e_ = ElmPtr( a.bml_e_->Clone() );
 
 
   (*pcerr) << "*** WARNING *** \n"
            << "*** WARNING *** bmlnElmnt::bmlnElmnt( bmlnElmnt const& )\n"
            << "*** WARNING *** The element pointed to by bml_e_ does not exist\n"
            << "*** WARNING *** within the beamline pointed to by p_bml_ .\n"
            << "*** WARNING *** "
            << endl;

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
    p_bml_        = BmlPtr();      
    bml_e_        = ElmPtr();    
    propfunc_     = rhs.propfunc_;
    pinnedFrames_ = rhs.pinnedFrames_;
    ctRef_        = rhs.ctRef_;
    attributes_   = rhs.attributes_;
    tag_          = rhs.tag_;
    pAperture_    = rhs.pAperture_ ? rhs.pAperture_->Clone() : 0 ; 
    dataHook      = rhs.dataHook;


 // ---------------------------------------------------------------------------------
 // If neither the beamline or the "element of interest" bml_e_ are  defined, 
 // we are done, just return.  
 //----------------------------------------------------------------------------------

 if (( !rhs.p_bml_ ) && (!rhs.bml_e_)) return *this; 

 // ---------------------------------------------------------------------------------
 // If only the "element of interest" bml_e_ is defined, and the beamline p_bml_ is not,
 // clone the element of interest and return;
 //----------------------------------------------------------------------------------

 if( !rhs.p_bml_ ) {
    bml_e_ = (rhs.bml_e_) ? ElmPtr(rhs.bml_e_->Clone()) : ElmPtr(); 
    return *this;
 } 

 // -------------------------------------------------------------------------------------------------------------------------
 // If we get here, the beamline p_bml_ is defined. The element of interest may or may not be defined. Typically, 
 // bml_e_ would be defined for elements modeled with a *single* thin kick in the middle, but not for elements 
 // modeled with multiple thin kicks. 
 //---------------------------------------------------------------------------------------------------------------------------

 p_bml_ = BmlPtr( rhs.p_bml_->Clone() );

 if (!bml_e_) return *this;  // there is no element of interest ... were are done 


 beamline::iterator it  =   p_bml_->begin(); 

 // find the position of the element of interest in the orginal beamline. The set the element of interest in the new one  
 // to the element with the same position

 for (  beamline::iterator ita  = rhs.p_bml_->begin(); ita != rhs.p_bml_->end(); ++ita, ++it ) {

   if( (*ita) == rhs.bml_e_ ) { bml_e_ = (*it);  return *this; }  // element of interest found. All OK. We are done.
 }
    
 //--------------------------------------------------------------------------------------------------------------------------
 // If we get here, this means that the element of interest exists, but was not found within the beamline. This is 
 // most likely a bug. Clone the element, and continue nevertheless, and  issue a warning.   
 //--------------------------------------------------------------------------------------------------------------------------

 bml_e_ = ElmPtr( rhs.bml_e_->Clone() );
 
 
   (*pcerr) << "*** WARNING *** \n"
            << "*** WARNING *** bmlnElmnt::operator=( bmlnElmnt const& )\n"
            << "*** WARNING *** The element pointed to by bml_e_ does not exist\n"
            << "*** WARNING *** within the beamline pointed to by p_bml_ .\n"
            << "*** WARNING *** "
            << endl;

   return *this;

}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::~bmlnElmnt() {

 // virtual method releasePropFunc() must
 //   be invoked within the derived class's destructor
 //   if it is necessary.


  dataHook.clear();

  if(align_)     delete align_;
  if(pAperture_) delete pAperture_;

  //  


}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// Begin: basic propagator functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


void bmlnElmnt::propagate( Particle& x ) 
{
  if( align_ == 0 ) {
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
  if( align_ == 0 ) {
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

  if( align_ == 0 ) {
    localPropagate  ( x );
  }
  else {

    for ( ParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { enterLocalFrame( **it ); }

    localPropagate  ( x );

    for ( ParticleBunch::iterator it = x.begin();  it != x.end(); ++it) { leaveLocalFrame( **it ); }

  }
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// End: basic propagator functions
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setLength( double const& x ) {
  if( length_ < 0.0 ) {
    (*pcerr) << "*** WARNING ***                       \n"
            "*** WARNING *** bmlnElmnt::setLength  \n"
            "*** WARNING *** Lengths must be positive.  \n"
            "*** WARNING *** You have entered "
         << x 
         << "    \n"
            "*** WARNING *** I will use its absolute\n"
            "*** WARNING *** value.                \n"
            "*** WARNING ***                       \n"
         << endl;
    length_ = -x;
  }
  else {
    length_ = x;
  }

  if( 0 != propfunc_ ) { this->setupPropFunc(); }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setStrength( double const& s ) {
  strength_ = s - getShunt()*IToField(); 
  if( 0 != propfunc_ ) { this->setupPropFunc(); }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setCurrent( double const& I ) {
  setStrength((I-getShunt()) * IToField());
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::releasePropFunc()
{
  ostringstream uic;
  uic  << "Base class method called by "
       << this->Type()
       << " object.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__
         , "void bmlnElmnt::releasePropFunc()"
         , uic.str().c_str() ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setupPropFunc()
{
  ostringstream uic;
  uic  << "Base class method called by "
       << this->Type()
       << " object.";
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__
         , "void bmlnElmnt::setupPropFunc()"
         , uic.str().c_str() ) );
}



//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc* a ) 
{ 
  bmlnElmnt::PropFunc* ret = this->propfunc_;

  if( 0 == a ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "bmlnElmnt::setPropFunction(const PropFunc* a)",
           "Argument is null." ) );
  }
  else {
    this->releasePropFunc();  // virtual
    this->propfunc_ = const_cast<PropFunc*>(a);
    this->setupPropFunc();    // virtual
  }

  return ret;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::PropFunc* bmlnElmnt::setPropFunction ( const PropFunc& a ) 
{ 
  return this->setPropFunction( &a );
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

bool bmlnElmnt::equivTo( const bmlnElmnt& x ) const 
{
  static double maxLength;
  static double maxStrength;

  if( typeid(*this) !=  typeid(x) ) {
    return false;
  }

  if( length_   < x.length_   ) { maxLength   = x.length_;   }
  else                        { maxLength   =   length_;   }
  if( strength_ < x.strength_ ) { maxStrength = x.strength_; }
  else                        { maxStrength =   strength_; }

  return ( ( fabs( length_   - x.length_   ) < 1.0e-6 * maxLength   )  &&
           ( fabs( strength_ - x.strength_ ) < 1.0e-6 * maxStrength )     );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::equivTo( const bmlnElmnt* x ) const 
{
  return this->equivTo( *x );
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
  return ( (0 == p_bml_) && (0 == bml_e_)  );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::isMagnet() const
{
  return false;
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
  if( this->hasParallelFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(u, 0.0, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.xOffset += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  if( this->hasParallelFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(0.0, u, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.yOffset += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  if( this->hasParallelFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(u, 0.0, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.xOffset = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  if( this->hasParallelFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(0.0, u, 0.0);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.yOffset = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  return (this->alignRelX( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignRelYmm( double const& u )
{
  return (this->alignRelY( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsXmm( double const& u )
{
  return (this->alignAbsX( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsYmm( double const& u )
{
  return (this->alignAbsY( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignRelRoll( double const& u )
{
  bool ret = true;
  if( this->hasParallelFaces() && this->hasStandardFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.tilt += u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double const& u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  if( this->hasParallelFaces() && this->hasStandardFaces() ) {
    if( 0 == align_ ) {
      align_ = new alignment(0.0, 0.0, u);
    }
    else {
      // This is stupid!
      alignmentData w(this->Alignment());
      w.tilt = u;
      this->setAlignment( w );
    }
  }
  else {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::alignRelX( double const& u )"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  return (this->alignRelRoll( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::alignAbsRollmrad( double const& u )
{
  return (this->alignAbsRoll( 0.001*u ));
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::realign()
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::realign is not finished.
  // #error *** WARNING ***

  if(0 != align_ ) 
  { delete align_; align_ = 0; }

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

  _pinnedFrameSet.reset();
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

void bmlnElmnt::loadPinnedCoordinates( const Particle& prtcl, Vector& ret, double pct )
{
  // #error *** WARNING ***
  // #error *** WARNING ***  bmlnElmnt::loadPinnedCoordinates is not tested!!
  // #error *** WARNING ***

  static Vector p( Particle::PSD / 2 );
  static Vector v( Particle::PSD / 2 );
  //                         |
  //                         +--------- Not foolproof.

  static int x  = Particle::xIndex();
  static int y  = Particle::yIndex();
  static int z  = Particle::cdtIndex();
  //                         |
  //                         +--------- Not quite what I want.
  static int xp = Particle::npxIndex();
  static int yp = Particle::npyIndex();
  static int zp = Particle::ndpIndex();
  //                         |
  //                         +--------- Not quite what I want.


  if( pinnedFrames_._altered ) 
  {
    if( pct < 0.000001 ) { pct = 0; }
    if( 0.999999 < pct ) { pct = 1; }

    Frame  ref;
    if( Particle::PSD == ret.Dim() ) {
      // Load position and momentum vectors
      p(x) = prtcl.get_x();   
      p(y) = prtcl.get_y();   
      p(z) = 0;

      v(x) = prtcl.get_npx(); 
      v(y) = prtcl.get_npy(); 
      v(z) = prtcl.get_npz();

      // Downstream end (default)
      if( (1.0 - pct) < 0.001 ) {
        pinnedFrames_._downStream.convertInPlace( p, v );
      }

      // Upstream end
      else if( pct < 0.001 ) {
        pinnedFrames_._upStream.convertInPlace( p, v );
      }

      // Somewhere in the middle
      else {
        Frame ref( Frame::tween( pinnedFrames_._upStream, pinnedFrames_._downStream, pct, false ) );
        ref.convertInPlace( p, v );
      }

      // Transfer the answer
      ret(x)  = p(x);
      ret(y)  = p(y);
      ret(z)  = prtcl.get_cdt();
      ret(xp) = v(x);
      ret(yp) = v(y);
      ret(zp) = prtcl.get_ndp();
    }  

    else {
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
    }
  }

  else // bmlnElmnt Frames have not been altered
  {
    ret(x)  = prtcl.get_x();
    ret(y)  = prtcl.get_y();
    ret(z)  = prtcl.get_cdt();
    ret(xp) = prtcl.get_npx();
    ret(yp) = prtcl.get_npy();
    ret(zp) = prtcl.get_ndp();
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool bmlnElmnt::setAlignment(alignmentData const& a) 
{
  bool ret = true;
  alignment* nuAlignPtr = new alignment(a);

  this->realign();

  if( nuAlignPtr->isNull() ) {
    delete nuAlignPtr;
    nuAlignPtr = 0;
  }
  else if( this->hasParallelFaces() ) {
    if( 0.0 == a.tilt ) {
      if( (a.xOffset != 0.0) || (a.yOffset != 0.0) ) {
        this->align_ = nuAlignPtr;
      }
    }
    else if( this->hasStandardFaces() ) {
      this->align_ = nuAlignPtr;
    }
    else { 
      ret = false;
    }
  }
  else {
    ret = false;
  }

  if( !ret ) {
    (*pcerr) << "\n*** WARNING *** "
         << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** bool bmlnElmnt::setAlignment(const alignmentData& a)"
            "\n*** WARNING *** Cannot use this method on an element "
         << this->Type() << "  " << this->Name()
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
  double temp;
  double cs, sn;

  cs = align_->cos_roll();
  sn = align_->sin_roll();

  Vector inState = p.getState();

  inState[0] -= align_->x_offset();
  inState[1] -= align_->y_offset();

  if( align_->roll() != 0.0) {
    double temp       = inState[0] * cs + inState[1] * sn;
    inState[1] = inState[1] * cs - inState[0] * sn;
    inState[0] = temp;

    temp       = inState[3] * cs + inState[4] * sn;
    inState[4] = inState[4] * cs - inState[3] * sn;
    inState[3] = temp;
  }
 
  p.setState( inState ) ;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::enterLocalFrame( JetParticle& p ) const
{
  double    cs, sn;

  cs = align_->cos_roll();
  sn = align_->sin_roll();

  JetVector inState = p.getState();

  inState(0) -= align_->x_offset();
  inState(1) -= align_->y_offset();

  if( align_->roll() != 0.0) {
    Jet temp       = inState(0) * cs + inState(1) * sn;
    inState(1) = inState(1) * cs - inState(0) * sn;
    inState(0) = temp;

    temp       = inState(3) * cs + inState(4) * sn;
    inState(4) = inState(4) * cs - inState(3) * sn;
    inState(3) = temp;
  }

  p.setState( inState );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::leaveLocalFrame( Particle& p ) const
{

  static double cs, sn;

  cs = align_->cos_roll();
  sn = align_->sin_roll();

  Vector outState = p.getState();

  if( align_->roll() != 0.0) {
    double temp        = outState[0] * cs - outState[1] * sn;
    outState[1] = outState[1] * cs + outState[0] * sn;
    outState[0] = temp;

    temp        = outState[3] * cs - outState[4] * sn;
    outState[4] = outState[4] * cs + outState[3] * sn;
    outState[3] = temp;
  }

  outState[0] += align_->x_offset();
  outState[1] += align_->y_offset();

  p.setState( outState );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::leaveLocalFrame( JetParticle& p ) const
{

  double cs = align_->cos_roll();
  double sn = align_->sin_roll();

  JetVector outState = p.getState();

  if( align_->roll() != 0.0) {
    Jet temp    = outState[0] * cs - outState[1] * sn;
    outState[1] = outState[1] * cs + outState[0] * sn;
    outState[0] = temp;

    temp        = outState[3] * cs - outState[4] * sn;
    outState[4] = outState[4] * cs + outState[3] * sn;
    outState[3] = temp;
  }

  outState[0] += align_->x_offset();
  outState[1] += align_->y_offset();

  p.setState( outState );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setAperture( Aperture* pAperture_in ) {
    //
    // aperture = x;
    //
  pAperture_ = pAperture_in;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::setReferenceTime( const Particle& prtn )
{
  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << "\n*** WARNING *** File: " << __FILE__ << ", Line: " << __LINE__
         << "\n*** WARNING *** double bmlnElmnt::setReferenceTime( const Particle& )"
            "\n*** WARNING *** Invoked on base class. Nothing will happen."
            "\n*** WARNING *** This warning is issued only once."
            "\n*** WARNING *** "
         << endl;
    firstTime = false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::getReferenceTime() const
{
  return ctRef_;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double bmlnElmnt::setReferenceTime( double const& x )
{
  double xtmp = x;

  double oldValue = ctRef_;
  if( fabs(xtmp) < 1.0e-12 ) { xtmp = 0.0; }
  ctRef_ = xtmp;
  if( p_bml_   ) {   p_bml_->setReferenceTime( xtmp ); }
  if( bml_e_ ) { bml_e_->setReferenceTime( xtmp ); }
  return oldValue;
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
  if( p_bml_ || bml_e_ ) {
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double const& pc,  ElmPtr& a, ElmPtr& b )", 
           "Cannot use base ::Split function when the element is composite." ) );
  }

  if( ( pc <= 0.0 ) || ( pc >= 1.0 ) ) {
    ostringstream uic;
    uic  << "Requested percentage = " << pc << "; not within [0,1].";
    throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
           "void bmlnElmnt::Split( double const& pc, ElmPtr& a, ElmPtr& b )", 
           uic.str().c_str() ) );
  }

  a = ElmPtr( a->Clone() );
  b = ElmPtr( b->Clone() );

  a->ident_ = ident_ + string("_1") ;
  b->ident_ = ident_ + string("_2") ;
  
  a->strength_ = pc*strength_;
  b->strength_ = ( 1.0 - pc )*strength_;
  a->length_   = pc*length_;
  b->length_   = ( 1.0 - pc )*length_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void bmlnElmnt::setShunt(double const& a) 
{               // Set the value of the shunt, creating it if necessary
  // REMOVE: strength_ += ( shuntCurrent_ - a ) * IToField();
  this->setStrength( strength_ + ( shuntCurrent_ - a ) * IToField() );
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

bmlnElmnt::Discriminator::Discriminator()
{
  // This does nothing, but its presence seems
  // to be needed by constructors of derived classes.
  // Go figure!
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::Discriminator::Discriminator( const Discriminator& )
{
  throw( bmlnElmnt::GenericException( __FILE__, __LINE__, 
         "bmlnElmnt::Discriminator::Discriminator( const Discriminator& )", 
         "Copy constructor invoked for a Discriminator object." ) );
}


// Exceptions

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bmlnElmnt::GenericException::GenericException( string fileName, int lineNumber, 
                                              const char* fcn, 
                                              const char* msg )
{
  ostringstream uic;
  uic << "\n*** ERROR *** "
         "\n*** ERROR *** File: " << fileName << ", Line: " << lineNumber
      << "\n*** ERROR *** " << fcn
      << "\n*** ERROR *** " << msg
      << "\n*** ERROR *** ";
  errorString = uic.str();

  static bool firstTime = true;
  if( firstTime ) {
    (*pcerr) << errorString;
    (*pcerr) << "\n*** ERROR *** This message is printed only once."
            "\n*** ERROR *** "
         << endl;
    firstTime = false;
  }
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* 
bmlnElmnt::GenericException::what() const throw()
{
  return errorString.c_str();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

boost::any& 
bmlnElmnt::operator[](const std::string& s) {

  return attributes_[s.c_str()]; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool  
bmlnElmnt::attributeExists( const std::string& s ) {

  return (attributes_.end() !=  attributes_.find( s.c_str() ) );


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  
bmlnElmnt::attributeClear( const std::string& s ) {

  attributes_.erase( s.c_str() ); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void  
bmlnElmnt::attributeClear() {

  attributes_.clear();

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


