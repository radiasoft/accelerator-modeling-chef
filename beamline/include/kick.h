/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      kick.h
******                                                                
******  Copyright Universities Research Association, Inc./ Fermilab    
******            All Rights Reserved                             
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
**************************************************************************
*************************************************************************/


#ifndef KICK_H
#define KICK_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <beamline/BmlVisitor.h>


class DLLEXPORT hkick : public bmlnElmnt
{
public:
  hkick();
  hkick( double const& );                                          // kick size in radians
  hkick( const char* name);                                 // name; assumes zero kick
  hkick( const char* name,                 double const& kick);     // kick size in radians
  hkick( const char* name,  double const& length, double const& kick);     // kick size in radians
  hkick( const hkick& );

  virtual hkick* Clone() const { return new hkick( *this ); }

  virtual ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;

  void accept( BmlVisitor& v ) { v.visitHkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitHkick( this ); }
};


class DLLEXPORT vkick : public bmlnElmnt
{
public:
  vkick();                                               // Assumes zero kick
  vkick( double const& );                                       // kick size in radians
  vkick( const char* );                                  // name; assumes zero kick
  vkick( const char* name, double const& kick);                  // kick size in radians
  vkick( const char*,     double const& length, double const& kick );  // kick size in radians
  vkick( const vkick& );

  vkick* Clone() const { return new vkick( *this ); }

  virtual ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVkick( this ); }

  const char* Type() const;
};


class DLLEXPORT kick : public bmlnElmnt {
private:
        double horizontalKick;
        double verticalKick;
        std::istream& readFrom(std::istream&);
        std::ostream& writeTo(std::ostream&);
public:
        kick();
        kick( char const* name );
        kick(                                           double const& horizontal_kick,double const& vertical_kick);
        kick( char const* name,                         double const& horizontal_kick, double const& vertical_kick);
        kick(                     double const& length, double const& horizontal_kick, double const& vertical_kick);
        kick( char const* name,   double const& length, double const& horizontal_kick, double const& vertical_kick);
        kick( kick const& );
        kick* Clone() const { return new kick( *this ); }

       virtual ~kick();

        void localPropagate( ParticleBunch& x) {bmlnElmnt::localPropagate( x ); }
        void localPropagate( Particle& );
        void localPropagate( JetParticle& );

        void accept(BmlVisitor& v)            { v.visitKick( this ); }
        void accept(ConstBmlVisitor& v) const { v.visitKick( this ); }

        double& horizontalStrength() { return horizontalKick; }
        double& verticalStrength()   { return verticalKick; }

        const char* Type() const;
 };

#endif    // KICK_H
