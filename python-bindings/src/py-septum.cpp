#include <boost/python.hpp>
#include <septum.h>

void wrap_septum () {
  

using namespace boost::python;


class_<thinSeptum, bases<bmlnElmnt> >("septum")
  .def(init<char*>() )
  .def(init<char*, double, double, double>() )
  .def(init<double, double, double>() )
  .def("setStrengths",  &thinSeptum::setStrengths)
  .def("setWire",       &thinSeptum::setWire)
  .def("Type",          &thinSeptum::Type);


}


#if 0

class thinSeptum : public bmlnElmnt
{
private:
  double strengthPos;    // kick in strength in radians for x > xWire
  double strengthNeg;	 // kick in strength in radians for x < xWire
  double xWire;		 // position of wire septum in meters
  std::ostream& writeTo(std::ostream&);
  std::istream& readFrom(std::istream&);

public:
  
  thinSeptum();
  
  thinSeptum( char*  n );
  
  thinSeptum( char*  n,      // name
	    double sPos,    // kick in strength in radians for x > xWire 
	    double sNeg,    // kick in strength in radians for x < xWire 
	    double x );     // position of wire septum in meters
   
  thinSeptum( double sPos,    
	    double sNeg,    
	    double x ); 
  
  thinSeptum( const thinSeptum& );
  ~thinSeptum();
  
  void setStrengths( double sPos, double sNeg); 
  void setWire( double x); 
  
  void localPropagate( ParticleBunch& x ) { bmlnElmnt::localPropagate( x ); }
  void localPropagate( Particle&    p );
  void localPropagate( JetParticle& p );

  void accept( BmlVisitor& v ) { v.visitThinSeptum( this ); }
  void accept( ConstBmlVisitor& v ) const { v.visitThinSeptum( this ); }

  void eliminate();
  
  const char* Type() const;
  virtual int isType(char* c) { if ( strcmp(c, "thinSeptum") != 0 ) return bmlnElmnt::isType(c); else return 1; }

  bmlnElmnt* Clone() const { return new thinSeptum( *this ); }

};
 
#endif
