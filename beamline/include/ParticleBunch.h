/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      ParticleBunch.h
******                                                                
******  Copyright (c) 2007 Fermi Research Alliance, Inc.    
******                All Rights Reserved                             
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******   
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
**************************************************************************
*************************************************************************/
#ifndef PARTICLEBUNCH_H
#define PARTICLEBUNCH_H

#include <stdlib.h>
#include <vector>
#include <algorithm>

#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/Distribution.h>
#include <beamline/Particle.h>

#include <boost/shared_ptr.hpp>

class ParticleBunch;

// stream operators

std::ostream& operator<<( std::ostream &os, ParticleBunch const& bunch);
std::istream& operator>>( std::istream &is, ParticleBunch&       bunch);


class ParticleBunch {


 friend std::istream& operator >>( std::istream &is, ParticleBunch& bunch);


public:
  
  typedef std::vector<ParticlePtr>::iterator             iterator;  
  typedef std::vector<ParticlePtr>::const_iterator const_iterator;

  enum PhaseSpaceProjection { x_npx=0, y_npy, ct_dpp };

  ParticleBunch( Particle const& reference, int nparticles=0);
 ~ParticleBunch();
  
  void append( Particle    const& p );  
  void append( ParticlePtr const& p );  

  void            setReferenceParticle ( Particle const& p);
  Particle const& getReferenceParticle () const;

  void populateGaussian ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateWaterBag ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateParabolic( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);
  void populateKV       ( PhaseSpaceProjection psid, double sigma_x, double sigma_np, double r_12=0.0);

  void populateBinomial ( PhaseSpaceProjection psid, double M, double x_lim, double px_lim, double r_12=0.0);

  void clear();

  std::vector<ParticlePtr> remove( );
   
  int size()   const;
  bool empty() const;

  // iterators 
 
  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end()   const;


private:

  std::vector<ParticlePtr>  bunch_;
  ParticlePtr               reference_;

};



#endif // PARTBUNCH_H
