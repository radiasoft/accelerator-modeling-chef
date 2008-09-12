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

alignmentData::alignmentData()
 : xOffset(0.0), yOffset(0.0), roll(0.0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData::alignmentData(double const& xoff, double const& yoff, double const& t)
: xOffset( xoff ), yOffset( yoff ), roll( t ) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData::alignmentData( alignmentData const& o)
: xOffset(o.xOffset), yOffset(o.yOffset), roll(o.roll) 
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
  roll    = data.roll;

  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignmentData::operator==( alignmentData const& data ) const
{
  return ( (yOffset == data.yOffset) && 
           (xOffset == data.xOffset) && 
           (roll    == data.roll   ) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment()
: adata_(0.0, 0.0, 0.0 ), cosRoll_(1.0), sinRoll_(0.0) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(double const& x, double const& y, double const& t ) 
  : adata_(x, y, t), cosRoll_(cos(t)), sinRoll_(sin(t))
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment( alignment const& o) 
: adata_(o.adata_), cosRoll_(o.cosRoll_), sinRoll_(o.sinRoll_) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::alignment(alignmentData const& o)
  :  adata_(o) 
{
 
     cosRoll_ = cos(adata_.roll),
     sinRoll_ = sin(adata_.roll);
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment::~alignment() 
{}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::isNull() const
{
  return ( ( adata_.yOffset == 0.0) && (adata_.xOffset == 0.0) && (adata_.roll == 0.0) );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool alignment::operator==( alignment const& o ) const
{
  return ( adata_ == o.adata_ );
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignment& alignment::operator=(alignment const& o) {

  if ( &o == this) return *this;

  adata_   = o.adata_;
  cosRoll_ = o.cosRoll_;
  sinRoll_ = o.sinRoll_;

  return *this;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void alignment::setAlignment(alignmentData const& data) 
{

  adata_   = data;

  cosRoll_ = cos( data.roll );
  sinRoll_ = sin( data.roll );
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

alignmentData const& alignment::getAlignment()  const 
{
  return adata_;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, alignment& align)
{
#if 0
  if ( &align ) 
    os << OSTREAM_DOUBLE_PREC << align.xOffset_ << " " << align.yOffset_ << " " << align.roll_;
  else
    os << "0 0 0 ";

#endif
  return os;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

