/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   1.0                    
******                                    
******  File:      FindCovariance.h
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


#ifndef FINDCOVARIANCE_H
#define FINDCOVARIANCE_H

class WIREData : public BarnacleData {
public:
  float hwires[48];
  float vwires[48];
  float hmean;
  float hsigma;
  float vmean;
  float vsigma;
  WIREData() {}
  ~WIREData() {}
};

MatrixD FindCovariance( const beamline& , const slist& , JetProton );
void    TestCovariance( const beamline& , const slist& , JetProton ,
		        const MatrixD& );

#endif
