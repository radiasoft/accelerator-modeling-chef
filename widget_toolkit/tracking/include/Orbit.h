/*************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******        interfaces to exercise the functionality of BEAMLINE.
******                                                                
******  File:      Orbit.h
******                                                                
******  Copyright (c) 2009  Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******                                                                
******  Author:    Leo Michelotti                                     
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******  REVISION HISTORY
******  ----------------
******  February, 2004.
******  - Original version of class Orbit introduced in
******  files widget_toolkit/include/Tracker.h and 
******  widget_toolkit/src/Tracker.cc.
****** 
******  May, 2008.
******  - Upgraded version of class Orbit written
******  as part of lpsDemo and stored in separate files.
******  
******  December, 2009.
******  - lpsDemo and Tracker versions merged to create
******  this one.
******  
**************************************************************************
*************************************************************************/

#ifndef ORBIT_H
#define ORBIT_H

#include <list>

#include <basic_toolkit/VectorD.h>

class Orbit
{
  public: 
    Orbit( Vector const & );  // Arguments are always cloned.
    Orbit( Vector const * );
    Orbit( Orbit  const & );   // Does a deep copy
    ~Orbit();

    void add( const Vector& );
    void add( const Vector* );

    const Vector* lastPoint();
    const Vector* firstPoint();

    double Red()   const;
    double Green() const;
    double Blue()  const;

    void setColor( double, double, double );

    enum RepStyle { points=0, lines } mode;

    typedef std::list<Vector*>::iterator             iterator;  
    typedef std::list<Vector*>::const_iterator const_iterator;  

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end()   const;

    std::list<Vector*> history_;  // TEMPORARILY PUBLIC

    long int maxNumberOfOrbits() const;
    void set_maxNumberOfOrbits( long int );
    void unset_maxNumberOfOrbits();
    static long int maxNumberOfOrbitsDefault();
    static void set_maxNumberOfOrbitsDefault( long int );
    static void unset_maxNumberOfOrbitsDefault();
    bool filled() const;

  private:
    double           red_;
    double           green_;
    double           blue_;
    long int         max_;
    static long int  defaultMax_;
};


// Inlined functions

inline double Orbit::Red() const
{
  return red_;
}

inline double Orbit::Green() const
{
  return green_;
}

inline double Orbit::Blue() const
{
  return blue_;
}

inline long int Orbit::maxNumberOfOrbits() const
{
  return max_;
}

#endif // ORBIT_H
