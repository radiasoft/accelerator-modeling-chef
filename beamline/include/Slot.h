/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      Slot.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
******                                                                
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
******    REVISION HISTORY
******
****** Mar 2007            ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
****** Dec 2007            ostiguy@fnal.gov
****** - new typesafe propagators
****** Apr 2008            michelotti@fnal.gov
****** - added placeholder Slot::setLength method
****** May 2008 ostiguy@fnal
****** - proper, explicit assignment operator
****** - propagator moved (back) to base class
****** - no assumption about internal structure
******
**************************************************************************
*************************************************************************/

#ifndef SLOT_H
#define SLOT_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <basic_toolkit/Frame.h>

class BmlVisitor;
class ConstBmlVisitor;

class Slot;

typedef boost::shared_ptr<Slot> SlotPtr;
 
class DLLEXPORT Slot : public bmlnElmnt {

  class Propagator;

 public:

   Slot();
   Slot( char const*  name);

   Slot( char const*  name, Frame const& out );

   Slot( const Slot& );

   ~Slot();
 
   Slot* Clone() const
     { return new Slot( *this ); }

   Slot& operator=( Slot const& other); 

   void makeUpstreamHorizontal   ( double const& length, double const& angle );
   void makeDownstreamHorizontal ( double const& length, double const& angle );
   void makeUpstreamVertical     ( double const& length, double const& angle );
   void makeDownstreamVertical   ( double const& length, double const& angle );

   int  checkFrame( const Frame& ) const;

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   void Split( double const& s, ElmPtr&, ElmPtr& ) const;

   bool         isMagnet()  const;
   bool         isThin()    const;
   bool         isPassive() const;

   const char*  Type()      const;

   int  setInFrame( Frame const& );
   int setOutFrame( Frame const& );

   Frame const& getInFrame() const
     { return in_; }
   Frame const& getOutFrame() const
     { return out_; }

   double OrbitLength( Particle const& );

 private:

   Frame in_;
   Frame out_;

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
};

#endif // SLOT_H
