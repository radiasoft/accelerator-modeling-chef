#ifndef PINGER_H
#define PINGER_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef BMLVISITOR_H
#include "BmlVisitor.h"
#endif

class Pinger : public bmlnElmnt {
private:
  int _armed;			/* Boolean: is it armed, ready to fire? */
  double _kick_direction;	/* In which direction is the kick? */
public:
  Pinger(       	/* Assumes a zero, horizontal kick */ );
  Pinger( double, 	/* kick size in radians,  */
	  double =0 	/* direction of kick, radians (H default) */ );
  Pinger( char * 	/* name */
	  /* Assumes zero, horizontal kick */ );
  Pinger( char * 	/* name */,
	  double	/* kick size in radians */,
	  double =0 	/* Horizontal kick is default*/ );
  Pinger( const Pinger& );
  Pinger( bmlnElmntData& );
  ~Pinger(); 
  const char* Type() const;
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );
  bmlnElmnt* Clone() const { return new Pinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitPinger( this ); }
  int arm() { _armed = 1; return _armed; }
  int disarm() { _armed = 0; return _armed; }
  int isArmed() { return _armed; };
  
  ostream& writeTo(ostream&);
  istream& readFrom(istream&);
  
  double getKickDirection() { return _kick_direction; }
  void setKickDirection(double k) { _kick_direction = k; }
};

class HPinger : public Pinger {
 private:
  int _armed;
 public:
  HPinger(       /* Assumes zero kick */ );
  HPinger( double /* kick size in radians */ );
  HPinger( char * /* name */  /* Assumes zero kick */ );
  HPinger( char * /* name */, double /* kick size in radians */ );
  HPinger( const HPinger& );
  HPinger( bmlnElmntData& );
  ~HPinger();
  const char* Type() const;
  bmlnElmnt* Clone() const { return new HPinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitHPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitHPinger( this ); }

  ostream& writeTo(ostream& os) { return os; }
  istream& readFrom(istream& is) { return is; } 
};

class VPinger : public Pinger {
 private:
  int _armed;
 public:
  VPinger(       /* Assumes zero kick */ );
  VPinger( double /* kick size in radians */ );
  VPinger( char * /* name */  /* Assumes zero kick */ );
  VPinger( char * /* name */, double /* kick size in radians */ );
  VPinger( const VPinger& );
  VPinger( bmlnElmntData& );
  ~VPinger();
  const char* Type() const;
  bmlnElmnt* Clone() const { return new VPinger( *this ); }
  void accept( BmlVisitor& v ) { v.visitVPinger( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitVPinger( this ); }

  ostream& writeTo(ostream& os) { return os; }
  istream& readFrom(istream& is) { return is; } 
};

#endif /* PINGER_H */
