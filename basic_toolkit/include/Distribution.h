/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BASIC TOOLKIT:  Low level utility C++ classes.
******  Version:   4.0                    
******                                    
******  File:      Distribution.h
******                                                                
******  Copyright (c) 1990 Universities Research Association, Inc.    
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
 * Distribtion class written by Jim Holt.
 * August 11, 1994
 *
 */

#ifndef DISTRIBUTION_HXX
#define DISTRIBUTION_HXX

extern "C" void   srand48( long );
extern "C" double drand48( void );

class Distribution {
protected:
  long initialSeed;
public:
  Distribution(long seed) {initialSeed = seed; srand48(seed);}
  virtual ~Distribution() { }
  virtual double getValue() { return drand48();}
};

class Gaussian : public Distribution {
public:
  Gaussian(long);
  virtual ~Gaussian();
  virtual double getValue();
};

#endif  // DISTRIBUTION_HXX
