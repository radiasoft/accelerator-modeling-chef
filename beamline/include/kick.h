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
****** REVISION HISTORY
******
****** Mar 2007:          ostiguy@fnal.gov
****** - use covariant return types
****** - support for reference counted elements
******
**************************************************************************
*************************************************************************/


#ifndef KICK_H
#define KICK_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>

class BmlVisitor;
class ConstBmlVisitor;


class hkick;

typedef boost::shared_ptr<hkick>        HKickPtr;
typedef boost::shared_ptr<hkick const>  ConstHKickPtr;

class vkick;
typedef boost::shared_ptr<vkick>        VKickPtr;
typedef boost::shared_ptr<vkick const>  ConstVKickPtr;

class kick;
typedef boost::shared_ptr<kick>        KickPtr;
typedef boost::shared_ptr<kick const>  ConstKickPtr;

class DLLEXPORT hkick : public bmlnElmnt {

public:

  hkick();
  hkick( double const& );                                          // kick size in radians
  hkick( const char* name);                                 // name; assumes zero kick
  hkick( const char* name,                 double const& kick);     // kick size in radians
  hkick( const char* name,  double const& length, double const& kick);     // kick size in radians
  hkick( const hkick& );

  virtual hkick* Clone() const { return new hkick( *this ); }

  hkick& operator=( hkick const& rhs);

  virtual ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type()       const;
  bool        isMagnet()   const;

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;


};



class DLLEXPORT vkick : public bmlnElmnt {

public:
  vkick();                                                             // Assumes zero kick
  vkick( double const& );                                              // kick size in radians
  vkick( const char* );                                                // name; assumes zero kick
  vkick( const char* name, double const& kick);                        // kick size in radians
  vkick( const char*,     double const& length, double const& kick );  // kick size in radians
  vkick( const vkick& );

  vkick* Clone() const { return new vkick( *this ); }

  vkick& operator=( vkick const& rhs);

  virtual ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v );
  void accept( ConstBmlVisitor& v ) const;

  const char* Type()       const;
  bool        isMagnet()   const;
};



class DLLEXPORT kick : public bmlnElmnt {

public:
        kick();
        kick( char const* name );
        kick(                                           double const& horizontal_kick, double const& vertical_kick);
        kick( char const* name,                         double const& horizontal_kick, double const& vertical_kick);
        kick(                     double const& length, double const& horizontal_kick, double const& vertical_kick);
        kick( char const* name,   double const& length, double const& horizontal_kick, double const& vertical_kick);
        kick( kick const& );

        kick* Clone() const { return new kick( *this ); }

        kick& operator=( kick const& rhs);

        virtual ~kick();

        void localPropagate( ParticleBunch& x) {bmlnElmnt::localPropagate( x ); }
        void localPropagate( Particle& );
        void localPropagate( JetParticle& );

        void accept(BmlVisitor& v);
        void accept(ConstBmlVisitor& v) const;

        double& horizontalStrength() { return horizontalKick_; }
        double& verticalStrength()   { return verticalKick_; }

        const char* Type()       const;
        bool        isMagnet()   const;

private:

        double horizontalKick_;
        double verticalKick_;

        std::istream& readFrom(std::istream&);
        std::ostream& writeTo(std::ostream&);

 };

#endif    // KICK_H
