/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                     
******                                    
******  File:      BunchProjector.cc
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

#include <beamline/BunchProjector.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Particle.h>

#include <algorithm>
#include <boost/shared_ptr.hpp>


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::SliceData::SliceData()
  : npart(0), xbar(0.0), ybar(0.0), x2bar(0.0), y2bar(0.0)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::SliceData::SliceData( int n, double x, double y, double x2, double y2)
  : npart(n), xbar(x), ybar(y), x2bar(x2), y2bar(y2)
{}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool LPositionOrder( ParticlePtr const& lhs,  ParticlePtr const& rhs )
{ 
  return  ( lhs->get_cdt() < rhs->get_cdt() ); 
}


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::BunchProjector( ParticleBunch& bunch, int nbins )
  :  histogram_( nbins, SliceData() ), 
      monopole_( nbins, double()    ), 
    dipole_hor_( nbins, double()    ),     
    dipole_ver_( nbins, double()    )
{

   std::sort(  bunch.begin(), bunch.end(), &LPositionOrder );

   cdt_min_     =    (*bunch.begin()  )->get_cdt(); 
   cdt_max_     =    (* --bunch.end() )->get_cdt();  
  
   populateHistograms( bunch, cdt_min_, cdt_max_, nbins);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::BunchProjector( ParticleBunch& bunch, double const& length, int nbins )
  :  histogram_( nbins, SliceData() ), 
      monopole_( nbins, double()    ), 
    dipole_hor_( nbins, double()    ),     
    dipole_ver_( nbins, double()    )
{

   std::sort(  bunch.begin(), bunch.end(), &LPositionOrder );

   cdt_min_     =    (*bunch.begin()  )->get_cdt(); 
   cdt_max_     =    cdt_min_ + length;
  
   populateHistograms( bunch, cdt_min_, cdt_min_ + length, nbins);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BunchProjector::populateHistograms( ParticleBunch& bunch, double const& smin,  double const& smax, int nbins)
{
   double position = smin; 
   double binsize  = (smax-smin) / nbins;

   std::vector<SliceData>::iterator  itslice         =  histogram_.begin();
   std::vector<double>::iterator     it_monopole     =   monopole_.begin();
   std::vector<double>::iterator     it_dipole_hor   = dipole_hor_.begin();
   std::vector<double>::iterator     it_dipole_ver   = dipole_ver_.begin();

   ParticleBunch::iterator itb = bunch.begin(); 

   while ( itb != bunch.end() ) {

    ParticlePtr p = *itb;

    position  += cdt_binsize_; 

    while (  p->get_cdt() <=  position )  { 

       p = *itb;
 
       ++(itslice->npart);

       itslice->xbar += p->get_x();
       itslice->ybar += p->get_y();

#if 0
============================================================
       itslice->x2bar += ( p->get_x() ) * ( p->get_x() );
       itslice->y2bar += ( p->get_y() ) * ( p->get_y() );
============================================================
#endif

       ++itb; 

       if ( itb ==  bunch.end() ) break;
      
    }   
 

    int npslice  = itslice->npart;

    if ( itslice->npart == 0  ) npslice = 1; // this prevents division by zero
                                             // the numerators should be zero so the 
                                             // results should be the same  
      
    *it_monopole    =  static_cast<double>( itslice->npart )/ static_cast<double>(bunch.size()); 


    itslice->xbar   /= npslice;
    itslice->ybar   /= npslice;


    *it_dipole_hor  =  itslice->xbar; 
    *it_dipole_ver  =  itslice->ybar;           

#if 0
====================================
    itslice->x2bar  /= npslice;
    itslice->y2bar  /= npslice;
====================================
#endif
          
    if ( itb ==  bunch.end() ) break;

    ++itslice; ++it_monopole; ++it_dipole_hor;  ++it_dipole_ver; 
    
  }

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BunchProjector::debug( ParticleBunch const& bunch) const
{

   int sum = 0;

   for ( std::vector<SliceData>::const_iterator it = histogram_.begin(); it != histogram_.end(); ++it ) 
     sum += it->npart;
 
   std::cout << "----------------------------------------------------------------------------------" << std::endl;
   std::cout << "No of particles computed by summing slices   = " << sum          << std::endl;
   std::cout << "No of particles as reported by bunch.szie()  = " << bunch.size() << std::endl;

   std::cout << "----------------------------------------------------------------------------------" << std::endl;
   std::cout << "Line density " << std::endl;
   std::cout << "----------------------------------------------------------------------------------" << std::endl;

   int idx = 0;
   for ( std::vector<double>::const_iterator it=monopole_.begin();  it != monopole_.end(); ++it, ++idx ) {

      std::cout << idx << "   " << *it << std::endl; 

  }
  std::cout << "----------------------------------------------------------------------------------" << std::endl;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& BunchProjector::monopoleLineDensity() const 
{

  return monopole_;

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& BunchProjector::dipoleHorLineDensity() const
{
  return dipole_hor_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

std::vector<double> const& BunchProjector::dipoleVerLineDensity() const
{
  return dipole_ver_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::~BunchProjector()
{ }


//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
