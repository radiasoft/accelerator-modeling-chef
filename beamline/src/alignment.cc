/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
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
****** REVISION HISTORY
****** 
****** Jan 2009  ostiguy@fnal.gov
****** - refactored: redundant class alignmentData has been eliminated
****** - added support for pitch angle
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

Alignment::Alignment()
  : xOffset_(0.0), yOffset_(0.0), roll_(0.0), pitch_(0.0), cosRoll_(1.0), sinRoll_(0.0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment::Alignment(double const& x, double const& y, double const& t, double const& p ) 
  : xOffset_(x), yOffset_(y), roll_(t), pitch_(p)
{
  cosRoll_ = cos(t);
  sinRoll_ = sin(t);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment::Alignment( Alignment const& o) 
: xOffset_(o.xOffset_), yOffset_(o.yOffset_), roll_(o.roll_), pitch_(o.pitch_), 
  cosRoll_(o.cosRoll_), sinRoll_(o.sinRoll_) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment::~Alignment() 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Alignment::isNull() const
{
  return ( ( yOffset_ == 0.0) && ( xOffset_ == 0.0) && ( roll_ == 0.0) );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

Alignment& Alignment::operator=( Alignment const& o ) 
{
  if ( this == &o ) return *this;

  xOffset_ = o.xOffset_;
  yOffset_ = o.yOffset_;
  roll_    = o.roll_;
  pitch_   = o.pitch_;
  sinRoll_ = o.sinRoll_;
  cosRoll_ = o.cosRoll_;

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool Alignment::operator==(Alignment const& o) const
{
  return (  (xOffset_ == o.xOffset_) && 
            (yOffset_ == o.yOffset_) && 
            (roll_    == o.roll_ )   &&
            (pitch_   == o.pitch_ ) ); 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::ostream& operator<<(ostream& os, Alignment const& align)
{
  os << align.xOffset() << " " << align.yOffset() << " " << align.roll() << endl;
  return os;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

