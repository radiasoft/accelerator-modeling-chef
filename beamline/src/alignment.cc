#ifdef __VISUAL_CPP__
#include <iomanip>
#else
#include <iomanip.h>
#endif

#include "beamline.inc"
#include "VectorD.h"

alignmentData::alignmentData() {
  xOffset = 0.0;
  yOffset = 0.0;
  tilt    = 0.0;
}

alignmentData::alignmentData(const alignmentData& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
}

alignmentData::~alignmentData() {
}

alignmentData& alignmentData::operator=(const alignmentData& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
  return *this;
}

alignment::alignment() {
  xOffset = 0.0;
  yOffset = 0.0;
  tilt    = 0.0;
  cosTilt = 1.0;
  sinTilt = 0.0;
}

alignment::alignment(const double x, const double y, const double t ) {
  xOffset = x;
  yOffset = y;
  tilt    = t;
  cosTilt = cos(t);
  sinTilt = sin(t);
}

alignment::alignment(const alignment& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
}

alignment::alignment(const alignmentData& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
  cosTilt = cos(tilt);
  sinTilt = sin(tilt);
}

alignment::~alignment() {
}

alignment& alignment::operator=(const alignment& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
  cosTilt = x.cosTilt;
  sinTilt = x.sinTilt;
  return *this;
}

void alignment::misalign(const Particle& p, BMLN_posInfo /* geometry */, 
			 double* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state[i];
  
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

void alignment::align(const Particle& p, BMLN_posInfo /* geometry */, 
			 double* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state[i];
  

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

void alignment::align(double* p, BMLN_posInfo /* geometry */, 
			 double* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
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

void alignment::misalign(JetParticle& p, BMLN_posInfo /* geometry */, 
			 Jet* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state(i);
  
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

void alignment::align(JetParticle& p, BMLN_posInfo /* geometry */, 
			 Jet* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
    inState[i] = p.state(i);
  

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

void alignment::align(Jet* p, BMLN_posInfo /* geometry */, 
			 Jet* inState) {
  int i;

  for( i = 0; i < BMLN_dynDim; i++  ) 
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

void alignment::setAlignment(const alignmentData& x) {
  xOffset = x.xOffset;
  yOffset = x.yOffset;
  tilt    = x.tilt;
  cosTilt = cos(tilt);
  sinTilt = sin(tilt);
}

alignmentData alignment::getAlignment() {
  alignmentData z;

  z.xOffset = xOffset;
  z.yOffset = yOffset;
  z.tilt    = tilt;

  return z;
}

ostream& operator<<(ostream& os, alignment& align)
{
  if ( &align ) 
    os << OSTREAM_DOUBLE_PREC << align.xOffset << " " << align.yOffset << " " << align.tilt;
  else
    os << "0 0 0 ";

  return os;
}
