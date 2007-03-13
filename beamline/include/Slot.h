/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
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
****** Mar 2007            ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
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
 

class DLLEXPORT Slot : public bmlnElmnt
{
 public:
   Slot();
   Slot( char const*  name);

   Slot(                    Frame const& out );
   Slot( char const*  name, Frame const& out );

   Slot(                    Frame const& in, Frame const& out, beamline const& );
   Slot( char const*  name, Frame const& in, Frame const& out, beamline const& );

   Slot(                    Frame const&  in, Frame const& out, bmlnElmnt const&);
   Slot( char const*  name, Frame const&  in, Frame const& out, bmlnElmnt const&);

   Slot( const Slot& );

   ~Slot();
 
   void makeUpstreamHorizontal   ( double const& length, double const& angle );
   void makeDownstreamHorizontal ( double const& length, double const& angle );
   void makeUpstreamVertical     ( double const& length, double const& angle );
   void makeDownstreamVertical   ( double const& length, double const& angle );

   int checkFrame( const Frame& ) const;

   void accept( BmlVisitor& v );
   void accept( ConstBmlVisitor& v ) const;

   void enterLocalFrame( Particle&     ) const;
   void enterLocalFrame( JetParticle&  ) const;
   void leaveLocalFrame( Particle&     ) const;
   void leaveLocalFrame( JetParticle&  ) const;
 
   void localPropagate( Particle& );
   void localPropagate( JetParticle& );
   void localPropagate( ParticleBunch& );
 
   Slot* Clone() const
     { return new Slot( *this ); }

   void Split( double const& s, ElmPtr&, ElmPtr& ) const;

   const char*  Type()  const;

   int  setInFrame( Frame const& );
   int setOutFrame( Frame const& );

   Frame getInFrame() const
     { return in; }
   Frame getOutFrame() const
     { return out; }

   // Not overloaded: virtual double getReferenceTime() const {return _ctRef;}

   double setReferenceTime(   double const& );
   double setReferenceTime( Particle const& );  // returns _ctRef

   // Functions passed on to tenant

   void setStrength   ( double const& );
   void setStrength   ( double const&, int );
   void setCurrent    ( double const& );
   bool setAlignment  ( const alignmentData& );
   double Current() const;
   double OrbitLength( Particle const& );

   std::string  Name() const;


 private:

   Frame in;
   Frame out;

   void processFrame( const Frame&, Particle& ) const;
   void processFrame( const Frame&, JetParticle& ) const;

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
};

#endif // SLOT_H
