/*
 *  This "lattice function" method is based on 
 *  D.A.Edwards and L.C.Teng
 *  "Parametrization of Linear Coupled Motion in Periodic Systems"
 *  IEEE Tr.NucSci.
 *  NS-20(3), 885(1973).
 *       -- Leo Michelotti
 *          June 6, 1994   ( D-Day + 50 years )
 */

#ifndef LATTFUNCSAGE_H
#define LATTFUNCSAGE_H

#ifndef BEAMLINE_H
#include "beamline.h"
#endif

class LattFuncSage;

typedef char (*ET_CRITFUNC)( bmlnElmnt* );
typedef int  (*ET_CALCFUNC)( LattFuncSage*, JetParticle*, ET_CRITFUNC ); 

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



class LattFuncSage {
private:
        beamline*  _myBeamline;
 static double     _csH, _csV, _snH, _snV;
 static Mapping*   _theMap;
        double     _dpp;    // used for dispersion calculations
        char       _verbose;

 ET_CALCFUNC       _theCalculator;
 
 static int attachETLattFuncs( bmlnElmnt* );

 static char no ( bmlnElmnt* );
 static char yes( bmlnElmnt* );

 static int Orig_RX_Calc ( LattFuncSage*, JetParticle* = 0, ET_CRITFUNC = 0 );
 static int Fast_CS_Calc ( LattFuncSage*, JetParticle* = 0, ET_CRITFUNC = 0 );
 static int Disp_Calc    ( LattFuncSage*, JetParticle* = 0, ET_CRITFUNC = 0 );
 static int ET_Disp_Calc ( LattFuncSage*, JetParticle* = 0, ET_CRITFUNC = 0 );
 static int CS_Disp_Calc ( LattFuncSage*, JetParticle* = 0, ET_CRITFUNC = 0 );
               // If default value is used for ET_CRITFUNC, then
               // information is attached to all elements.

public:
 LattFuncSage( const beamline* );
 ~LattFuncSage();
  
 int doCalc( ET_CRITFUNC = 0 );
 int doCalc( JetParticle*, ET_CRITFUNC = 0 ); 
 int doCalc( const lattFunc&, JetParticle&, ET_CRITFUNC = 0 ); 

 void eraseAll();
 void set_dpp( double );
 double get_dpp();

 void set_ET_Calc();
 void set_CS_Calc();
 void set_Disp_Calc();
 void set_ET_Disp_Calc();
 void set_CS_Disp_Calc();

 void set_verbose();
 void unset_verbose();

};


inline int LattFuncSage::doCalc( JetParticle* p_jp, ET_CRITFUNC Crit )
{
  return _theCalculator( this, p_jp, Crit );
}


inline void LattFuncSage::set_ET_Calc()
{
  _theCalculator = LattFuncSage::Orig_RX_Calc;
}


inline void LattFuncSage::set_CS_Calc()
{
  _theCalculator = LattFuncSage::Fast_CS_Calc;
}

inline void LattFuncSage::set_Disp_Calc()
{
  _theCalculator = LattFuncSage::Disp_Calc;
}

inline void LattFuncSage::set_ET_Disp_Calc()
{
  _theCalculator = LattFuncSage::ET_Disp_Calc;
}

inline void LattFuncSage::set_CS_Disp_Calc()
{
  _theCalculator = LattFuncSage::CS_Disp_Calc;
}


inline double LattFuncSage::get_dpp()
{
  return _dpp;
}


inline void LattFuncSage::set_verbose()
{
  _verbose = 1;
}

inline void LattFuncSage::unset_verbose()
{
  _verbose = 0;
}



#endif // LATTFUNCSAGE_H
