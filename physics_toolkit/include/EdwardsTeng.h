/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.1
******                                    
******  File:      EdwardsTeng.h
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


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

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef MAP_HXX
#include "Mapping.h"
#endif

#ifndef MATHCONS_H
#include "MathConstants.h"
#endif


typedef char (*ET_CRITFUNC)( bmlnElmnt* );

struct ETinfo : BarnacleData 
{
 double arcLength;
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

 ETinfo() : D(2,2), EV(6,6), arcLength(-1.0) { }
 ETinfo( const ETinfo& );
 ~ETinfo(){}
};


struct ETtunes : BarnacleData 
{
 double hor;
 double ver;
 ETtunes() : hor(0.0), ver(0.0) {}
 ETtunes( const ETtunes& x ) {hor = x.hor; ver = x.ver;}
 ~ETtunes(){}
};


class EdwardsTeng 
{
private:
 beamline* myBeamline;
 static double csH, csV, snH, snV;
 static Mapping* theMap;
 static int attachETLattFuncs( bmlnElmnt* );
public:
 EdwardsTeng( const beamline* );
 ~EdwardsTeng();
 int doCalc( void* ptr = 0, ET_CRITFUNC = 0 ); 
                            // If default value is used for ET_CRITFUNC, then
                            // information is attached to all elements.
 void eraseAll();
};

#endif // EDWARDSTENG_H
