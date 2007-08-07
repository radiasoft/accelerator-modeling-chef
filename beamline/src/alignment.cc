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
: xOffset(0.0), yOffset(0.0), tilt(0.0), cosTilt(1.0), sinTilt(0.0) {}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(double x, double y, double t ) 
  : xOffset(x), yOffset(y), tilt(t) 
{

 cosTilt = cos(t);
 sinTilt = sin(t); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment( alignment const& a) 
: xOffset(a.xOffset), yOffset(a.yOffset), tilt(a.tilt), 
  cosTilt(a.cosTilt), sinTilt(a.sinTilt) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(alignmentData const& a)
  :  xOffset(a.xOffset), yOffset(a.yOffset), tilt(a.tilt) 
{
  cosTilt = cos(tilt);
  sinTilt = sin(tilt);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::~alignment() {}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::isNull() const
{
  return ( (yOffset == 0.0) && (xOffset == 0.0) && (tilt == 0.0) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::operator==( alignment const& a ) const
{
  return ( (yOffset == a.yOffset) && 
           (xOffset == a.xOffset) && 
           (tilt    == a.tilt   ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment& alignment::operator=(alignment const& a) {

  if ( &a == this) return *this;

  xOffset = a.xOffset;
  yOffset = a.yOffset;
  tilt    = a.tilt;
  cosTilt = a.cosTilt;
  sinTilt = a.sinTilt;

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign(Particle const& p, Vector& inState) {

  Vector const& state = const_cast<Particle&>(p).State();

  for( int i = 0; i < BMLN_dynDim; ++i  ) 
    inState[i] = state[i];
  
  inState[0] -= xOffset;
  inState[1] -= yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.inPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = geometry.inPoint - offset;
  // ??? REMOVE   inState[0] += r*geometry.inAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.inAxes[1];
  // ??? REMOVE }
  
  if(tilt != 0.0) {
    double result[4];
     result[0] = inState[0] * cosTilt + inState[1] * sinTilt;
     result[1] = inState[1] * cosTilt - inState[0] * sinTilt;

    inState[0] = result[0];
    inState[1] = result[1];
 
    result[2] = inState[3] * cosTilt + inState[4] * sinTilt;
    result[3] = inState[4] * cosTilt - inState[3] * sinTilt;

    inState[3] = result[2];
    inState[4] = result[3];
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(Particle const& p, Vector& inState) {


  Vector const& state = const_cast<Particle&>(p).State();

  for( int i=0; i < BMLN_dynDim; ++i  ) 
    inState[i] = state[i];
  

 if(tilt != 0.0) {
   double result[4];
     result[0] = inState[0] * cosTilt - inState[1] * sinTilt;
     result[1] = inState[1] * cosTilt + inState[0] * sinTilt;

    inState[0] = result[0];
    inState[1] = result[1];

     result[2] = inState[3] * cosTilt - inState[4] * sinTilt;
     result[3] = inState[4] * cosTilt + inState[3] * sinTilt;

    inState[3] = result[2];
    inState[4] = result[3];
 }

  inState[0] += xOffset;
  inState[1] += yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.outPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = offset - geometry.outPoint;
  // ??? REMOVE   inState[0] += r*geometry.outAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.outAxes[1];
  // ??? REMOVE }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(Vector& p, Vector& inState) {


  for( int i = 0; i < BMLN_dynDim; ++i  ) 
    inState[i] = p[i];
  

 if(tilt != 0.0) {
   double result[4];
    result[0] = inState[0] * cosTilt - inState[1] * sinTilt;
    result[1] = inState[1] * cosTilt + inState[0] * sinTilt;
   
   inState[0] = result[0];
   inState[1] = result[1];

   result[2] = inState[3] * cosTilt - inState[4] * sinTilt;
   result[3] = inState[4] * cosTilt + inState[3] * sinTilt;

   inState[3] = result[2];
   inState[4] = result[3];
 }

  inState[0] += xOffset;
  inState[1] += yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.outPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = offset - geometry.outPoint;
  // ??? REMOVE   inState[0] += r*geometry.outAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.outAxes[1];
  // ??? REMOVE }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign(JetParticle& p, Mapping& inState) {

  Mapping& state = p.State();

  for( int i=0; i < BMLN_dynDim; ++i  ) 
    inState[i] = state(i);
  
  inState[0] -= xOffset;
  inState[1] -= yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.inPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = geometry.inPoint - offset;
  // ??? REMOVE   inState[0] += r*geometry.inAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.inAxes[1];
  // ??? REMOVE }
  
  if(tilt != 0.0) {
    Jet result[4];
    result[0] = inState[0] * cosTilt + inState[1] * sinTilt;
    result[1] = inState[1] * cosTilt - inState[0] * sinTilt;

    inState[0] = result[0];
    inState[1] = result[1];

    result[2] = inState[3] * cosTilt + inState[4] * sinTilt;
    result[3] = inState[4] * cosTilt - inState[3] * sinTilt;

    inState[3] = result[2];
    inState[4] = result[3];
  }

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(JetParticle& p, Mapping& inState) {

  Mapping& state = p.State();

  for( int i = 0; i < BMLN_dynDim; ++i  ) 
    inState[i] = state(i);
  

 if(tilt != 0.0) {
   Jet result[4];
   result[0] = inState[0] * cosTilt - inState[1] * sinTilt;
   result[1] = inState[1] * cosTilt + inState[0] * sinTilt;

   inState[0] = result[0];
   inState[1] = result[1];

   result[2] = inState[3] * cosTilt - inState[4] * sinTilt;
   result[3] = inState[4] * cosTilt + inState[3] * sinTilt;
   inState[3] = result[2];
   inState[4] = result[3];
 }

  inState[0] += xOffset;
  inState[1] += yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.outPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = offset - geometry.outPoint;
  // ??? REMOVE   inState[0] += r*geometry.outAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.outAxes[1];
  // ??? REMOVE }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align(JetVector& p, Mapping& inState) {

  for( int i = 0; i < BMLN_dynDim; ++i  ) 
    inState[i] = p[i];
  

 if(tilt != 0.0) {
   Jet result[4];
   result[0] = inState[0] * cosTilt - inState[1] * sinTilt;
   result[1] = inState[1] * cosTilt + inState[0] * sinTilt;

   inState[0] = result[0];
   inState[1] = result[1];

   result[2] = inState[3] * cosTilt - inState[4] * sinTilt;
   result[3] = inState[4] * cosTilt + inState[3] * sinTilt;

   inState[3] = result[2];
   inState[4] = result[3];
 }

  inState[0] += xOffset;
  inState[1] += yOffset;

  // ??? REMOVE if((xOffset != 0.0) || (yOffset != 0.0)) {
  // ??? REMOVE   offset = geometry.outPoint;
  // ??? REMOVE   offset(0) += xOffset;
  // ??? REMOVE   offset(1) += yOffset;
  // ??? REMOVE   r = offset - geometry.outPoint;
  // ??? REMOVE   inState[0] += r*geometry.outAxes[0];
  // ??? REMOVE   inState[1] += r*geometry.outAxes[1];
  // ??? REMOVE }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::setAlignment(alignmentData const& data) 
{

  xOffset = data.xOffset;
  yOffset = data.yOffset;

  tilt    = data.tilt;
  cosTilt = cos(tilt);
  sinTilt = sin(tilt);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData alignment::getAlignment()  const 
{
  alignmentData z;

  z.xOffset = xOffset;
  z.yOffset = yOffset;
  z.tilt    = tilt;

  return z;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, alignment& align)
{
  if ( &align ) 
    os << OSTREAM_DOUBLE_PREC << align.xOffset << " " << align.yOffset << " " << align.tilt;
  else
    os << "0 0 0 ";

  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign( Vector& inState )
{

  double result[4];

  inState[0] -= xOffset;
  inState[1] -= yOffset;

  if(tilt != 0.0) {
    result[0] = inState[0] * cosTilt + inState[1] * sinTilt;
    result[1] = inState[1] * cosTilt - inState[0] * sinTilt;
    inState[0] = result[0];
    inState[1] = result[1];
    result[2] = inState[3] * cosTilt + inState[4] * sinTilt;
    result[3] = inState[4] * cosTilt - inState[3] * sinTilt;
    inState[3] = result[2];
    inState[4] = result[3];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align( Vector& outState ) 
{
 if(tilt != 0.0) {
   double result[4];
   result[0] = outState[0] * cosTilt - outState[1] * sinTilt;
   result[1] = outState[1] * cosTilt + outState[0] * sinTilt;
   outState[0] = result[0];
   outState[1] = result[1];
   result[2] = outState[3] * cosTilt - outState[4] * sinTilt;
   result[3] = outState[4] * cosTilt + outState[3] * sinTilt;
   outState[3] = result[2];
   outState[4] = result[3];
 }

  outState[0] += xOffset;
  outState[1] += yOffset;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::misalign( JetVector& inState ) 
{
  inState(0) -= xOffset;
  inState(1) -= yOffset;

  if(tilt != 0.0) {
    Jet result[4];
    result[0] = inState(0) * cosTilt + inState(1) * sinTilt;
    result[1] = inState(1) * cosTilt - inState(0) * sinTilt;
    inState(0) = result[0];
    inState(1) = result[1];
    result[2] = inState(3) * cosTilt + inState(4) * sinTilt;
    result[3] = inState(4) * cosTilt - inState(3) * sinTilt;
    inState(3) = result[2];
    inState(4) = result[3];
  }
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::align( JetVector& outState ) 
{
 if(tilt != 0.0) {
   Jet result[4];
   result[0] = outState(0) * cosTilt - outState(1) * sinTilt;
   result[1] = outState(1) * cosTilt + outState(0) * sinTilt;
   outState(0) = result[0];
   outState(1) = result[1];
   result[2] = outState(3) * cosTilt - outState(4) * sinTilt;
   result[3] = outState(4) * cosTilt + outState(3) * sinTilt;
   outState(3) = result[2];
   outState(4) = result[3];
 }

  outState(0) += xOffset;
  outState(1) += yOffset;

}

