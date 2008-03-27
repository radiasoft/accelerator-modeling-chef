/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                     
******                                    
******  File:      BunchProjector.h
******                                                               
******  Copyright (c) Fermi Research Alliance LLC / Fermilab    
******                All Rights Reserved                             
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No.  DE-AC02-07CH11359 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov
******
******
******
**************************************************************************
**************************************************************************
**************************************************************************/

#ifndef BUNCHPROJECTOR_H
#define BUNCHPROJECTOR_H

#include <vector>

template <typename Particle_t> 
class TBunch;

class Particle;
typedef TBunch<Particle> ParticleBunch;


//----------------------------------------------------------------------------------
// Workaround for g++ < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------
#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))

#include <beamline/Particle.h>

struct  LWakeOrder {
  typedef  Particle const& first_argument_type;
  typedef  Particle const& second_argument_type;
  typedef  bool     result_type;

  bool operator()( Particle const&  lhs,  Particle const&  rhs ) const
   { 
     return  ( lhs.get_cdt() < rhs.get_cdt() );  
   }
};
#endif

// -----------------------------------------------------------------------------------

class BunchProjector {

 public:

    BunchProjector( ParticleBunch& bunch, int nsamples=128 );  
    BunchProjector( ParticleBunch& bunch, double const& interval, int nbins=128 );  
   ~BunchProjector();

    std::vector<double>  const& monopoleLineDensity()  const;
    std::vector<double>  const& dipoleHorLineDensity() const;
    std::vector<double>  const& dipoleVerLineDensity() const;

    void debug( ParticleBunch const& bunch) const;

    double cdt_min();
    double cdt_max();

 private:

    void populateHistograms( ParticleBunch& bunch, double const& smin, double const& smax, int nsamples );
 
    double cdt_min_;
    double cdt_max_;

    std::vector<double>                monopole_;
    std::vector<double>                dipole_hor_;
    std::vector<double>                dipole_ver_;



};

#endif //  BUNCHPROJECTOR_H


