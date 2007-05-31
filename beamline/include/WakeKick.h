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
#include <boost/bind.hpp>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;

class WakeKick;

typedef boost::shared_ptr<WakeKick>        WakeKickPtr;
typedef boost::shared_ptr<WakeKick const>  ConstWakeKickPtr;


class DLLEXPORT WakeKick : public bmlnElmnt {

  public:

   template <typename Propagator>  
   WakeKick( Propagator propagator);      

   template <typename Propagator>  
   WakeKick( char const* name, Propagator propagator);      

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

   boost::function< void( ParticleBunch& ) > propagator_;

} ;


template <typename Propagator>  
WakeKick::WakeKick( char const* name,  Propagator propagator )      
 : bmlnElmnt(name, 0.0, 0.0),
   propagator_(propagator)
{}


template <typename Propagator>  
WakeKick::WakeKick( Propagator propagator)      
 : bmlnElmnt("", 0.0, 0.0),
   propagator_(propagator)
{}



#endif // WAKEKICK_H
