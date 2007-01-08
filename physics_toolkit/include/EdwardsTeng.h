/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      EdwardsTeng.h
******  Version:   1.1
******                                                                
******  Copyright (c) 2001  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
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

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/MathConstants.h>
#include <basic_toolkit/Matrix.h>
#include <mxyzptlk/Mapping.h>
#include <beamline/bmlnElmnt.h>

typedef bool (*ET_CRITFUNC)( bmlnElmnt* );


struct ETinfo {

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

 ETinfo( ETinfo const& );

};


struct ETtunes {

 double hor;
 double ver;

 ETtunes() : hor(0.0), ver(0.0) {}

 ETtunes( ETtunes const & x ) : hor(x.hor), ver(x.ver) {}

};


class EdwardsTeng {

private:

public:

  EdwardsTeng( beamline const* );

 ~EdwardsTeng();
 
  int doCalc( JetParticle&, ET_CRITFUNC = 0 ); 
                            // If default value is used for ET_CRITFUNC, then
                            // information is attached to all elements.
  void eraseAll();

 private:

 static int attachETLattFuncs( bmlnElmnt* );

 beamline* myBeamline;

 static double   csH, csV, snH, snV;
 static Mapping* theMap;


};

#endif // EDWARDSTENG_H
