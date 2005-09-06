/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  MXYZPTLK:  A C++ implementation of differential algebra.      
******                                    
******  File:      Jet.h
******  Version:   5.0
******                                                                
******  Copyright (c) 1990, 2004 Universities Research Association, Inc.    
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

#ifndef JET_H
#define JET_H

#include <TJetEnvironment.h>
#include <TJL.h>
#include <TJet.h>

typedef TJetEnvironment<double,FNAL::Complex> Jet__environment;
typedef TJetEnvironment<FNAL::Complex,double> JetC__environment;

typedef TJLterm<double,FNAL::Complex> JLterm;
typedef TJLterm<FNAL::Complex,double> JLCterm;
typedef TJL<double,FNAL::Complex> JL;
typedef TJL<FNAL::Complex,double> JLC;

typedef TJet<double,FNAL::Complex> Jet;
typedef TJet<FNAL::Complex,double> JetC;
typedef Tcoord<double,FNAL::Complex> coord;
typedef Tcoord<FNAL::Complex,double> coordC;

#endif // JET_H
