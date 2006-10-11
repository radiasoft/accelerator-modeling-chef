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


class DLLEXPORT hkick : public bmlnElmnt
{
public:
  hkick();
  hkick( double );         // kick size in radians
  hkick( const char* );    // name; assumes zero kick
  hkick( const char*,      // name
         double );         // kick size in radians
  hkick( const char*,      // name
         double,           // length
         double );         // kick size in radians
  hkick( const hkick& );
  hkick( bmlnElmntData& );

  ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "hkick") != 0 ) return bmlnElmnt::isType(c); else return true; }
  virtual hkick* Clone() const { return new hkick( *this ); }

  void accept( BmlVisitor& v ) { v.visitHkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitHkick( this ); }
};


class DLLEXPORT vkick : public bmlnElmnt
{
public:
  vkick();                  // Assumes zero kick
  vkick( double );          // kick size in radians
  vkick( const char* );     // name; assumes zero kick
  vkick( const char*,       // name
         double  );         // kick size in radians
  vkick( const char*,       // name
         double,            // length
         double  );         // kick size in radians
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVkick( this ); }

  const char* Type() const;
  virtual bool isType(const char* c) 
  { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return true; }
  vkick* Clone() const { return new vkick( *this ); }
};


class DLLEXPORT kick : public bmlnElmnt {
private:
        double horizontalKick;
        double verticalKick;
        std::istream& readFrom(std::istream&);
        std::ostream& writeTo(std::ostream&);
public:
        kick();
        kick( const char* /* name */ );
        kick( double, /* horizontal kick */ double /* vertical kick */ );
        kick( const char*, double, double );
        kick( double, // length
              double, // horizontal kick 
              double  // vertical kick 
            );
        kick( const char*, // name 
              double,      // length
              double,      // horizontal kick 
              double       // vertical kick
            );
        kick( const kick& );
        virtual ~kick();

        void localPropagate( ParticleBunch& x) {bmlnElmnt::localPropagate( x ); }
        void localPropagate( Particle& );
        void localPropagate( JetParticle& );

        void accept(BmlVisitor& v) { v.visitKick( this ); }
        void accept(ConstBmlVisitor& v) const { v.visitKick( this ); }

        double& horizontalStrength() { return horizontalKick; }
        double& verticalStrength() { return verticalKick; }

        const char* Type() const;
        kick* Clone() const { return new kick( *this ); }
};

#endif    // KICK_H
