/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      WakeKick.h
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
******
**************************************************************************
*************************************************************************/

#ifndef WAKEKICK_H
#define WAKEKICK_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/ConvolutionFunctor.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class WakeKick;

typedef boost::shared_ptr<WakeKick>        WakeKickPtr;
typedef boost::shared_ptr<WakeKick const>  ConstWakeKickPtr;

class DLLEXPORT WakeKick : public bmlnElmnt {

  public:

   WakeKick(                         int nsamples = 1024, double convolution_interval = -1.0);      
   WakeKick( char const* name = "",  int nsamples = 1024, double convolution_interval = -1.0);      
   WakeKick( WakeKick const&         );

   WakeKick* Clone() const { return new WakeKick( *this ); }

   WakeKick& operator=( WakeKick const& rhs);

  ~WakeKick();

   void localPropagate( ParticleBunch& x );

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   const char* Type()     const;
   bool        isMagnet() const;

 private:

   void init();

   ConvolutionFunctor<double>   hor_twake_; 
   ConvolutionFunctor<double>   ver_twake_; 
   ConvolutionFunctor<double>   lwake_; 
   int                          nsamples_;
   double                       interval_;
} ;

#endif // WAKEKICK_H
