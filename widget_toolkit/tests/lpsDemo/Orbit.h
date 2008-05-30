////////////////////////////////////////////////////////////
// 
// File:          Orbit.h
//                                                                
// -------------------------------------------------------------
// 
// Copyright Universities Research Association, Inc./ Fermilab    
//           All Rights Reserved                             
//
// Usage, modification, and redistribution are subject to terms          
// of the License supplied with this software.
// 
// Software and documentation created under 
// U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
// The U.S. Government retains a world-wide non-exclusive, 
// royalty-free license to publish or reproduce documentation 
// and software for U.S. Government purposes. This software 
// is protected under the U.S. and Foreign Copyright Laws.
// 
// -------------------------------------------------------------
// 
// Author:        Leo Michelotti
// 
// REVISION HISTORY
// 
// Thursday. May 29, 2008.       (this file)
// 
////////////////////////////////////////////////////////////
// 
// A variation on class Orbit appearing implemented in
// widget_toolkit/tracking/src/Tracker.cc.  After further
// development, a successor of this file will eventually
// replace that implementation.
// 
////////////////////////////////////////////////////////////

#ifndef ORBIT_H
#define ORBIT_H

#include <list>

#include <basic_toolkit/VectorD.h>

namespace AESOP_domain
{

class Orbit
{
  public: 
    Orbit( const Vector& );  // Arguments are always cloned.
    Orbit( const Vector* );
    Orbit( Orbit const& );
    ~Orbit();

    void add( const Vector& );
    void add( const Vector* );

    const Vector* lastPoint();
    const Vector* firstPoint();

    double Red();
    double Blue();
    double Green();
    void setColor( double, double, double );

    typedef std::list<Vector*>::iterator             iterator;  
    typedef std::list<Vector*>::const_iterator const_iterator;  

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end()   const;

    std::list<Vector*> _history;  // TEMPORARILY PUBLIC

    long int maxNumberOfOrbits() const;
    void set_maxNumberOfOrbits( long int );
    void unset_maxNumberOfOrbits();
    static long int maxNumberOfOrbitsDefault();
    static void set_maxNumberOfOrbitsDefault( long int );
    static void unset_maxNumberOfOrbitsDefault();
    bool filled();

  private:
    double          _red;
    double          _green;
    double          _blue;
    long int        _max;
    static long int _defaultMax;
};


// Inlined functions

inline double Orbit::Red()
{
  return _red;
}

inline double Orbit::Green()
{
  return _green;
}

inline double Orbit::Blue()
{
  return _blue;
}

inline long int Orbit::maxNumberOfOrbits() const
{
  return _max;
}

};  // end namespace AESOP

#endif // ORBIT_H
