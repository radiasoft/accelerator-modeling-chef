   // from MXYZPLTK
#include <beamline.h>

static inline bmlnElmnt*
make_multipole( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_solenoid( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_elseparator( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_instrument( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_ecollimator( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_rcollimator( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_yrot( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_beambeam( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_matrix( char* name, double length ) {
  return new drift( name, length );
}

static inline bmlnElmnt*
make_lump( char* name, double length ) {
  return new drift( name, length );
}
