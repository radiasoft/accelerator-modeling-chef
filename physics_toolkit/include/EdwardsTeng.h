/*
 *  This "lattice function" method is based on 
 *  D.A.Edwards and L.C.Teng
 *  "Parametrization of Linear Coupled Motion in Periodic Systems"
 *  IEEE Tr.NucSci.
 *  NS-20(3), 885(1973).
 *       -- Leo Michelotti
 *          June 6, 1994   ( D-Day + 50 years )
 */

#ifndef EDWARDSTENG_H
#define EDWARDSTENG_H

#include "beamline.h"

typedef char (*ET_CRITFUNC)( bmlnElmnt* );

struct ETinfo : BarnacleData {
 Mapping map;
 Mapping mapInv;
 struct {
  double hor;
  double ver;
 } beta;
 struct {
  double hor;
  double ver;
 } alpha;
 double phi;
 MatrixD D;
 MatrixC EV;
 ETinfo() : D(2,2), EV(6,6) { }
 ETinfo( const ETinfo& );
 ~ETinfo(){}
};


struct ETtunes : BarnacleData {
 double hor;
 double ver;
 ETtunes() : hor(0.0), ver(0.0) {}
 ETtunes( const ETtunes& x ) {hor = x.hor; ver = x.ver;}
 ~ETtunes(){}
};


class EdwardsTeng {
private:
 beamline* myBeamline;
 static double csH, csV, snH, snV;
 static Mapping* theMap;
 friend int attachETLattFuncs( bmlnElmnt* );
public:
 EdwardsTeng( const beamline* );
 ~EdwardsTeng();
 int doCalc( void* ptr = 0, ET_CRITFUNC = 0 ); 
                            // If default value is used for ET_CRITFUNC, then
                            // information is attached to all elements.
 void eraseAll();
};

#endif // EDWARDSTENG_H
