#ifndef SLOT_H
#define SLOT_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef FRAME_H
#include "Frame.h"
#endif


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
 
   short int checkFrame( const Frame& ) const;

   ostream& writeTo ( ostream& );
   istream& readFrom( istream& );
   void image( int, slist*, BMLN_posInfo* );
 
   void eliminate() 
     { delete this; }
 
   void accept( BmlVisitor& v ) 
     { v.visitSlot( this ); }
 
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

#endif // SLOT_H
