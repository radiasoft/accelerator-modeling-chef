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

#include "Sage.h"

class LattFuncSage;

typedef char (*ET_CRITFUNC)( bmlnElmnt* );


class LattFuncSage : public Sage
{
public:
 LattFuncSage( const beamline* );
 ~LattFuncSage();
  
 int TuneCalc       ( JetParticle* );
 int Tune_Chrom_Calc( JetParticle* );
 int Disp_Calc      ( JetParticle*, ET_CRITFUNC = 0 );
 int Orig_RX_Calc   ( JetParticle*, ET_CRITFUNC = 0 );
		      
 int Fast_CS_Calc   ( JetParticle*, ET_CRITFUNC = 0 );
 int ET_Disp_Calc   ( JetParticle*, ET_CRITFUNC = 0 );
 int CS_Disp_Calc   ( JetParticle*, ET_CRITFUNC = 0 );
               // If default value is used for ET_CRITFUNC, then
               // information is attached to all elements.

 int Twiss_Calc     ( const lattFunc&, JetParticle&, ET_CRITFUNC = 0 ); 
 int Twiss_Calc     ( JetParticle& );
               // These reproduce the old beamline::twiss
               // and therefore are both obsolete and wrong.

 void eraseAll();

 void set_dpp( double );
 double get_dpp();


private:
 static double     _csH, _csV, _snH, _snV;
 static Mapping*   _theMapPtr;
        double     _dpp;    // used for dispersion calculations

 static int attachETLattFuncs( bmlnElmnt* );

};


inline double LattFuncSage::get_dpp()
{
  return _dpp;
}


#endif // LATTFUNCSAGE_H
