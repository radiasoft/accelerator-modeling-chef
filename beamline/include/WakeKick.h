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
******  May 2008 ostiguy@fnal.gov
******  - proper, explicit assignment operator
******  - propagator moved (back) to base class
******  - no assumption about internal structure
******
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef WAKEKICK_H
#define WAKEKICK_H

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/ConvolutionFunctor.h>
#include <boost/bind.hpp>
#include <beamline/bmlnElmnt.h>
#include <beamline/BasePropagator.h>

class BmlVisitor;
class ConstBmlVisitor;

class WakeKick;

typedef boost::shared_ptr<WakeKick>        WakeKickPtr;
typedef boost::shared_ptr<WakeKick const>  ConstWakeKickPtr;


class DLLEXPORT WakeKick : public bmlnElmnt {

private:

   class Propagator;  

public:

   WakeKick( std::string const& name );      

   WakeKick( WakeKick const& );

   WakeKick* Clone() const { return new WakeKick( *this ); }

  ~WakeKick();

   WakeKick& operator=( WakeKick const& rhs);

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   const char* Type()     const;
   bool        isMagnet() const;

} ;

#endif // WAKEKICK_H
