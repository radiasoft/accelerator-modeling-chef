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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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
 
   void makeUpstreamHorizontal   ( double /* length */, double /* angle */ );
   void makeDownstreamHorizontal ( double /* length */, double /* angle */ );
   void makeUpstreamVertical     ( double /* length */, double /* angle */ );
   void makeDownstreamVertical   ( double /* length */, double /* angle */ );

   short int checkFrame( const Frame& ) const;

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

   std::ostream& writeTo ( std::ostream& );
   std::istream& readFrom( std::istream& );
};

#endif // SLOT_H
