/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-kick.cpp
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
#include <kick.h>
#include <boost/python.hpp>

using namespace boost::python;

void wrap_kick () {

class_<hkick, bases<bmlnElmnt> >("hkick")
  .def(init<double>())
  .def(init<char*>())
  .def(init<char*,double>())
  .def(init<char*,double, double>())
  .def( "Type", &hkick::Type);


class_<vkick, bases<bmlnElmnt> >("vkick")
  .def(init<double>())
  .def(init<char*>())
  .def(init<char*,double>())
  .def(init<char*,double, double>())
  .def( "Type", &vkick::Type);


class_<kick,bases<bmlnElmnt> >("kick")
  .def(init<double,double>())
  .def(init<char*>())
  .def(init<char*,double, double, double>())
  .def(init<char*,double, double, double>())
  .def( "Type", &kick::Type);
}


#if 0

class hkick : public bmlnElmnt
{
public:
  hkick();
  hkick( double );   // kick size in radians
  hkick( char* );    // name; assumes zero kick
  hkick( char*,      // name
         double );   // kick size in radians
  hkick( char*,      // name
         double,     // length
         double );   // kick size in radians
  hkick( const hkick& );
  hkick( bmlnElmntData& );

  ~hkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "hkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new hkick( *this ); }

  void accept( BmlVisitor& v ) { v.visitHkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitHkick( this ); }
};


class vkick : public bmlnElmnt
{
public:
  vkick();            // Assumes zero kick
  vkick( double );    // kick size in radians
  vkick( char* );     // name; assumes zero kick
  vkick( char*,       // name
         double  );   // kick size in radians
  vkick( char*,       // name
         double,      // length
         double  );   // kick size in radians
  vkick( const vkick& );
  vkick( bmlnElmntData& );
  ~vkick();

  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );

  void accept( BmlVisitor& v ) { v.visitVkick( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVkick( this ); }

  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "vkick") != 0 ) return bmlnElmnt::isType(c); else return 1; }
  bmlnElmnt* Clone() const { return new vkick( *this ); }
};


class kick : public bmlnElmnt {
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
        bmlnElmnt* Clone() const { return new kick( *this ); }
};

#endif    // KICK_H

