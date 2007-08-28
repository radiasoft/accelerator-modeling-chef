/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      alignment.cc
******                                                                
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
*****
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
******                                                                
**************************************************************************
*************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include <beamline/Alignment.h>
#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/VectorD.h>
#include <beamline/Particle.h>
#include <beamline/JetParticle.h>

using namespace std;

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData::alignmentData(): xOffset(0.0), yOffset(0.0), tilt(0.0) 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData::alignmentData( alignmentData const& data)
: xOffset(data.xOffset), yOffset(data.yOffset), tilt(data.tilt) 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData::~alignmentData() {}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData& alignmentData::operator=(alignmentData const& data) 
{
  if ( &data == this ) return *this;

  xOffset = data.xOffset;
  yOffset = data.yOffset;
  tilt    = data.tilt;

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignmentData::operator==( alignmentData const& data ) const
{
  return ( (yOffset == data.yOffset) && 
           (xOffset == data.xOffset) && 
           (tilt    == data.tilt   ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment()
: xOffset_(0.0), yOffset_(0.0), tilt_(0.0), cosTilt_(1.0), sinTilt_(0.0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(double const& x, double const& y, double const& t ) 
  : xOffset_(x), yOffset_(y), tilt_(t), cosTilt_(cos(t)), sinTilt_(sin(t))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment( alignment const& a) 
: xOffset_(a.xOffset_), yOffset_(a.yOffset_), tilt_(a.tilt_), 
  cosTilt_(a.cosTilt_), sinTilt_(a.sinTilt_) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(alignmentData const& a)
  :  xOffset_(a.xOffset), yOffset_(a.yOffset), tilt_(a.tilt) 
{
 
     cosTilt_ = cos(tilt_),
     sinTilt_ = sin(tilt_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::~alignment() 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::isNull() const
{
  return ( (yOffset_ == 0.0) && (xOffset_ == 0.0) && (tilt_ == 0.0) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::operator==( alignment const& a ) const
{
  return ( (yOffset_ == a.yOffset_) && 
           (xOffset_ == a.xOffset_) && 
           (tilt_    == a.tilt_   ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment& alignment::operator=(alignment const& a) {

  if ( &a == this) return *this;

  xOffset_ = a.xOffset_;
  yOffset_ = a.yOffset_;
  tilt_    = a.tilt_;
  cosTilt_ = a.cosTilt_;
  sinTilt_ = a.sinTilt_;

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 
void alignment::misalign(Particle const& p, Vector& inState) {

  inState = p.State();

  inState[0] -= xOffset_;
  inState[1] -= yOffset_;

  
  if(tilt_ != 0.0) {
    double result[4];
    result[0] = inState[0] * cosTilt_ + inState[1] * sinTilt_;
    result[1] = inState[1] * cosTilt_ - inState[0] * sinTilt_;

    inState[0] = result[0];
    inState[1] = result[1];
 
    result[2] = inState[3] * cosTilt_ + inState[4] * sinTilt_;
    result[3] = inState[4] * cosTilt_ - inState[3] * sinTilt_;

    inState[3] = result[2];
    inState[4] = result[3];
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(Particle const& p, Vector& inState) {


  inState = p.State();

 if(tilt_ != 0.0) {
   double result[4];
     result[0] = inState[0] * cosTilt_ - inState[1] * sinTilt_;
     result[1] = inState[1] * cosTilt_ + inState[0] * sinTilt_;

    inState[0] = result[0];
    inState[1] = result[1];

     result[2] = inState[3] * cosTilt_ - inState[4] * sinTilt_;
     result[3] = inState[4] * cosTilt_ + inState[3] * sinTilt_;

    inState[3] = result[2];
    inState[4] = result[3];
 }

  inState[0] += xOffset_;
  inState[1] += yOffset_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(Vector const& p, Vector& inState) {

  inState = p;

  if(tilt_ != 0.0) {
    double result[4];
    result[0] = inState[0] * cosTilt_ - inState[1] * sinTilt_;
    result[1] = inState[1] * cosTilt_ + inState[0] * sinTilt_;
   
    inState[0] = result[0];
    inState[1] = result[1];

    result[2] = inState[3] * cosTilt_ - inState[4] * sinTilt_;
    result[3] = inState[4] * cosTilt_ + inState[3] * sinTilt_;

    inState[3] = result[2];
    inState[4] = result[3];
  }

  inState[0] += xOffset_;
  inState[1] += yOffset_;


}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign(JetParticle const& p, Mapping& inState) {

  inState = p.State();

  inState[0] -= xOffset_;
  inState[1] -= yOffset_;

  
  if(tilt_ != 0.0) {
    Jet result[4];
    result[0] = inState[0] * cosTilt_ + inState[1] * sinTilt_;
    result[1] = inState[1] * cosTilt_ - inState[0] * sinTilt_;

    inState[0] = result[0];
    inState[1] = result[1];

    result[2] = inState[3] * cosTilt_ + inState[4] * sinTilt_;
    result[3] = inState[4] * cosTilt_ - inState[3] * sinTilt_;

    inState[3] = result[2];
    inState[4] = result[3];
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(JetParticle const& p, Mapping& inState) {

  inState = p.State();

 if(tilt_ != 0.0) {
   Jet result[4];
   result[0] = inState[0] * cosTilt_ - inState[1] * sinTilt_;
   result[1] = inState[1] * cosTilt_ + inState[0] * sinTilt_;

   inState[0] = result[0];
   inState[1] = result[1];

   result[2] = inState[3] * cosTilt_ - inState[4] * sinTilt_;
   result[3] = inState[4] * cosTilt_ + inState[3] * sinTilt_;
   inState[3] = result[2];
   inState[4] = result[3];
 }

  inState[0] += xOffset_;
  inState[1] += yOffset_;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(JetVector const& p, Mapping& inState) {

  inState = p; 

 if(tilt_ != 0.0) {
   Jet result[4];
   result[0] = inState[0] * cosTilt_ - inState[1] * sinTilt_;
   result[1] = inState[1] * cosTilt_ + inState[0] * sinTilt_;

   inState[0] = result[0];
   inState[1] = result[1];

   result[2] = inState[3] * cosTilt_ - inState[4] * sinTilt_;
   result[3] = inState[4] * cosTilt_ + inState[3] * sinTilt_;

   inState[3] = result[2];
   inState[4] = result[3];
 }

  inState[0] += xOffset_;
  inState[1] += yOffset_;


}

#endif
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::setAlignment(alignmentData const& data) 
{

  xOffset_ = data.xOffset;
  yOffset_ = data.yOffset;

  tilt_    = data.tilt;
  cosTilt_ = cos(tilt_);
  sinTilt_ = sin(tilt_);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData alignment::getAlignment()  const 
{
  alignmentData z;

  z.xOffset = xOffset_;
  z.yOffset = yOffset_;
  z.tilt    = tilt_;

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, alignment& align)
{
  if ( &align ) 
    os << OSTREAM_DOUBLE_PREC << align.xOffset_ << " " << align.yOffset_ << " " << align.tilt_;
  else
    os << "0 0 0 ";

  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0
void alignment::misalign( Vector& inState )
{

  inState[0] -= xOffset_;
  inState[1] -= yOffset_;

  if(tilt_ != 0.0) {
 
    double result[4];

    result[0]  = inState[0] * cosTilt_ + inState[1] * sinTilt_;
    result[1]  = inState[1] * cosTilt_ - inState[0] * sinTilt_;
    inState[0] = result[0];
    inState[1] = result[1];
    result[2]  = inState[3] * cosTilt_ + inState[4] * sinTilt_;
    result[3]  = inState[4] * cosTilt_ - inState[3] * sinTilt_;
    inState[3] = result[2];
    inState[4] = result[3];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align( Vector& outState ) 
{

 if(tilt_ != 0.0) {
   double result[4];
   result[0] = outState[0] * cosTilt_ - outState[1] * sinTilt_;
   result[1] = outState[1] * cosTilt_ + outState[0] * sinTilt_;
   outState[0] = result[0];
   outState[1] = result[1];
   result[2] = outState[3] * cosTilt_ - outState[4] * sinTilt_;
   result[3] = outState[4] * cosTilt_ + outState[3] * sinTilt_;
   outState[3] = result[2];
   outState[4] = result[3];
 }

  outState[0] += xOffset_;
  outState[1] += yOffset_;


}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign( JetVector& inState ) 
{
  inState(0) -= xOffset_;
  inState(1) -= yOffset_;

  if(tilt_ != 0.0) {
    Jet result[4];
    result[0]  = inState(0) * cosTilt_ + inState(1) * sinTilt_;
    result[1]  = inState(1) * cosTilt_ - inState(0) * sinTilt_;
    inState(0) = result[0];
    inState(1) = result[1];
    result[2]  = inState(3) * cosTilt_ + inState(4) * sinTilt_;
    result[3]  = inState(4) * cosTilt_ - inState(3) * sinTilt_;
    inState(3) = result[2];
    inState(4) = result[3];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align( JetVector& outState ) 
{

 if(tilt_ != 0.0) {
   Jet result[4];
   result[0]   = outState(0) * cosTilt_ - outState(1) * sinTilt_;
   result[1]   = outState(1) * cosTilt_ + outState(0) * sinTilt_;
   outState(0) = result[0];
   outState(1) = result[1];
   result[2]   = outState(3) * cosTilt_ - outState(4) * sinTilt_;
   result[3]   = outState(4) * cosTilt_ + outState(3) * sinTilt_;
   outState(3) = result[2];
   outState(4) = result[3];
 }

  outState(0) += xOffset_;
  outState(1) += yOffset_;
}

#endif
