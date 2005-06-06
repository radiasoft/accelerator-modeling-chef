/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-slot.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
#include <boost/python.hpp>

#include <Slot.h>


void wrap_slot () {
  

using namespace boost::python;


class_<Slot, bases<bmlnElmnt> >("Slot")
  .def(init<const char*>())
  .def("makeUpstreamHorizontal",   &Slot::makeUpstreamHorizontal)
  .def("makeDownstreamHorizontal", &Slot::makeDownstreamHorizontal)
  .def("makeUpstreamVertical",     &Slot::makeUpstreamVertical)
  .def("makeDownstreamVertical",   &Slot::makeDownstreamVertical)
  .def( "Type", &Slot::Type);

}

#if 0

class Slot : public bmlnElmnt
{
 public:
   Slot();
   Slot( const char*  /* name */ );

   Slot( const Frame& /* out */
       );
   Slot( const char*  /* name */,
         const Frame& /* out */
       );

   Slot( const Frame& /* in */, 
         const Frame& /* out */, 
         const beamline&
       );
   Slot( const char*  /* name */,
         const Frame& /* in */, 
         const Frame& /* out */, 
         const beamline&
       );

   Slot( const Frame& /* in */, 
         const Frame& /* out */, 
         const bmlnElmnt&
       );
   Slot( const char*  /* name */,
         const Frame& /* in */, 
         const Frame& /* out */, 
         const bmlnElmnt&
       );

   Slot( const Slot& );

   ~Slot();
 
   void makeUpstreamHorizontal   ( double /* length */, double /* angle */ );
   void makeDownstreamHorizontal ( double /* length */, double /* angle */ );
   void makeUpstreamVertical     ( double /* length */, double /* angle */ );
   void makeDownstreamVertical   ( double /* length */, double /* angle */ );

   short int checkFrame( const Frame& ) const;

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
 
   void eliminate() 
     { delete this; }
 
   void accept( BmlVisitor& v ) { v.visitSlot( this ); }
   void accept( ConstBmlVisitor& v ) const { v.visitSlot( this ); }

   void enterLocalFrame( Particle&     ) const;
   void enterLocalFrame( JetParticle&  ) const;
   void leaveLocalFrame( Particle&     ) const;
   void leaveLocalFrame( JetParticle&  ) const;
 
   void localPropagate( Particle& );
   void localPropagate( JetParticle& );
   void localPropagate( ParticleBunch& );
 
   bmlnElmnt* Clone() const
     { return new Slot( *this ); }

   void Split( double s, bmlnElmnt**, bmlnElmnt** );

   const char*  Type()  const;

   short int setInFrame( const Frame& );
   short int setOutFrame( const Frame& );

   Frame getInFrame() const
     { return in; }
   Frame getOutFrame() const
     { return out; }

   // Not overloaded: virtual double getReferenceTime() const {return _ctRef;}
   double setReferenceTime( double );
   double setReferenceTime( const Particle& );  // returns _ctRef

   // Functions passed on to tenant
   void setStrength   ( double );
   void setStrength   ( double, int );
   void setCurrent    ( double );
   void setAlignment  ( const alignmentData& );
   double Current() const;
   double OrbitLength( const Particle& );
   const char* Name() const;


 private:
   Frame in;
   Frame out;

   void processFrame( const Frame&, Particle& ) const;
   void processFrame( const Frame&, JetParticle& ) const;
};

#endif
