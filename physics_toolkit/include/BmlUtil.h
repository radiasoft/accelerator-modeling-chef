/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******  Version:   2.0
******                                    
******  File:      BmlUtil.h
******                                                                
******  Copyright (c) 2003  Universities Research Association, Inc.   
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
 *  File: BmlUtil.h
 *  Header file for collecting Beamline
 *  utilities as methods of a single class.
 *  
 *  Leo Michelotti
 *  October 28, 2003
 */


class MatrixC;

class BmlUtil
{
 public: 
  static void normalize( MatrixC& E );
  // On input: the columns of E contain eigenvectors of
  //   a one-turn matrix. 
  // On output: each column has been normalized
  //   for physical interpretation.
 private: 
  static const double mlt1;
};
