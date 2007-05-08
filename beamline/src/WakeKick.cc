/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKick.cc
******
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <beamline/WakeKick.h>
#include <beamline/BmlVisitor.h>
#include <beamline/WakeFunctions.h>

using namespace std;


// **************************************************
//   class WakeKick 
// **************************************************

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::WakeKick( int nsamples, double interval )      
  : bmlnElmnt(""), nsamples_(nsamples), interval_(interval)  
{ 
  init();
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::WakeKick( char const* name, int nsamples, double interval)      
  : bmlnElmnt(name), nsamples_(nsamples), interval_(interval)  
{ 
  init();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::WakeKick( WakeKick const& x )
  : bmlnElmnt(x), nsamples_(x.nsamples_), interval_(x.interval_), 
                  hor_twake_(x.hor_twake_),  ver_twake_(x.ver_twake_), lwake_(x.lwake_) 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::init()
{ 

  double ds = interval_/ (nsamples_-1); 

  lwake_     = ConvolutionFunctor<double>( nsamples_,  ds, ShortRangeLWakeFunction(), true); 
  hor_twake_ = ConvolutionFunctor<double>( nsamples_,  ds, ShortRangeTWakeFunction(), true); 
  ver_twake_ = ConvolutionFunctor<double>( nsamples_,  ds, ShortRangeTWakeFunction(), true); 

}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

WakeKick::~WakeKick() 
{ }

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

const char* WakeKick::Type() const 
{ 
  return "WakeKick"; 
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool     WakeKick::isMagnet() const
{

  return false;

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::accept( BmlVisitor& v ) 
{ 
   v.visit(*this ); 
}
  
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void WakeKick::accept( ConstBmlVisitor& v ) const 
{ 
   v.visit(*this ); 
}
