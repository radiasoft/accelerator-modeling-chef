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

class ParticleBunch;

class BunchProjector {

  struct SliceData {
    SliceData();
    SliceData( int n, double x, double y, double x2, double y2);
    int    npart; 
    double xbar;
    double ybar;
    double x2bar;
    double y2bar;
  };


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

    std::vector<SliceData>             histogram_;
    std::vector<double>                monopole_;
    std::vector<double>                dipole_hor_;
    std::vector<double>                dipole_ver_;

};

#endif //  BUNCHPROJECTOR_H


