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
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <iomanip>

#include "bmlnElmnt.h"
#include "VectorD.h"
#include "Particle.h"

using namespace std;

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

bool alignmentData::operator==( const alignmentData& x ) const
{
  return ( (yOffset == x.yOffset) && 
           (xOffset == x.xOffset) && 
           (tilt    == x.tilt   ) );
}



alignment::alignment() {
  xOffset = 0.0;
  yOffset = 0.0;
  tilt    = 0.0;
  cosTilt = 1.0;
  sinTilt = 0.0;
}

alignment::alignment(double x, double y, double t ) {
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
  cosTilt = cos(tilt);
  sinTilt = sin(tilt);
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


bool alignment::isNull() const
{
  return ( (yOffset == 0.0) && (xOffset == 0.0) && (tilt == 0.0) );
}


bool alignment::operator==( const alignment& x ) const
{
  return ( (yOffset == x.yOffset) && 
           (xOffset == x.xOffset) && 
           (tilt    == x.tilt   ) );
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

alignmentData alignment::getAlignment()  const {
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


void alignment::misalign( double* inState )
{
  static double result[4];
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

void alignment::align( double* outState ) 
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

