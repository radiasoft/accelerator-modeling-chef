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

#include <basic_toolkit/iosetup.h>
#include <beamline/ParticleBunch.h>
#include <beamline/Particle.h>
#include <beamline/ParticleBunch.h>
#include <beamline/TBunch.h>
#include <beamline/BunchProjector.h>

#include <algorithm>
#include <boost/shared_ptr.hpp>

using FNAL::pcout;

//---------------------------------------------------------------------------------
// NOTE: When longitudinal projections are performed to compute wakefields. 
//       is important to remember that cdt > 0 implies that a particle arrives 
//       *later* than the reference. 
//
//       To compute a wake by convolution, we need to have the wake specified 
//       in the same order, that is z represents the distance behind the 
//       excitation !
//-----------------------------------------------------------------------------------


//----------------------------------------------------------------------------------
// Workaround for g++ < 4.2 mishandling of templates defined in anonymous namespace
//----------------------------------------------------------------------------------

#if (__GNUC__ == 3) ||  ((__GNUC__ == 4) && (__GNUC_MINOR__ < 2 ))
#endif

namespace {
 struct  LWakeOrder {

  typedef  Particle const& first_argument_type;
  typedef  Particle const& second_argument_type;
  typedef  bool     result_type;

  bool operator()( Particle const&  lhs,  Particle const&  rhs ) const
   {
     return  ( lhs.get_cdt() < rhs.get_cdt() );
   }
 };

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::BunchProjector( ParticleBunch& bunch, int nsamples )
  :   monopole_( nsamples, double()    ), 
    dipole_hor_( nsamples, double()    ),     
    dipole_ver_( nsamples, double()    )
{

   bunch.sort( (LWakeOrder()) );

   cdt_min_     =    (bunch.begin()  )->get_cdt(); 
   cdt_max_     =    ( --bunch.end() )->get_cdt();  
  
   populateHistograms( bunch, cdt_min_, cdt_max_, nsamples);  
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

BunchProjector::BunchProjector( ParticleBunch& bunch, double const& length, int nsamples )
  : monopole_( nsamples, double()    ), 
    dipole_hor_( nsamples, double()    ),     
    dipole_ver_( nsamples, double()    )
{

   bunch.sort( (LWakeOrder()) );
  
   cdt_min_     =    (bunch.begin())->get_cdt(); 
   cdt_max_     =    cdt_min_ + length;
  
   if (( (--bunch.end())->get_cdt() - bunch.begin()->get_cdt() ) > length ) {

     (*pcout) << "*** WARNING ***: BunchProjector: Sampled region is smaller than total bunch length." << std::endl;
     (*pcout) << "*** WARNING ***: BunchProjector: Sampled region width is " << length <<  std::endl;
     (*pcout) << "*** WARNING ***: BunchProjector: Bunch length is " 
              <<  ( (--bunch.end())->get_cdt() - bunch.begin()->get_cdt() ) <<  std::endl;
   }

   populateHistograms( bunch, cdt_min_, cdt_min_ + length, nsamples);  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BunchProjector::populateHistograms( ParticleBunch& bunch, double const& smin,  double const& smax, int nsamples)
{
   double binsize  = (smax-smin) / ( nsamples-1 );
   double position = smin - (0.5*binsize);     //  Note: the first bin is the interval  ] smin-0.5*binsize, smin+0.5*binsize ]  

   std::vector<double>::iterator     it_monopole     =   monopole_.begin();
   std::vector<double>::iterator     it_dipole_hor   = dipole_hor_.begin();
   std::vector<double>::iterator     it_dipole_ver   = dipole_ver_.begin();

   ParticleBunch::const_iterator itb = bunch.begin(); 

   double xsum = 0.0;
   double ysum = 0.0;

   int nslice = 0;

   while ( itb != bunch.end() ) {

       position  += binsize; 

       while (  itb->get_cdt() <=  position )  { 
      
         xsum += itb->get_x();
         ysum += itb->get_y();
   
         ++nslice;

         if ( ++itb ==  bunch.end() ) break; 

       } // while   
 
 
       *it_monopole    =  static_cast<double>( nslice )/ static_cast<double>(bunch.size()) / binsize; 
       *it_dipole_hor  =  xsum / bunch.size() / binsize; 
       *it_dipole_ver  =  ysum / bunch.size() / binsize;           

       if ( itb == bunch.end() ) break;

       xsum   = 0.0;
       ysum   = 0.0;
       nslice = 0;

       ++it_monopole; ++it_dipole_hor;  ++it_dipole_ver; 

       if ( it_monopole ==  monopole_.end()  ) { 
         (*pcout) << "*** WARNING ***: Not all particles included in the histogram.\n"
	             "*** WARNING ***: The sampled interval width is probably smaller than the total bunch length." << std::endl;
          break;  // We just exit after issuing a warning. Perhaps an execption should be thrown.
       }
 
  } // while

}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void BunchProjector::debug( ParticleBunch const& bunch) const
{

   std::cout << "----------------------------------------------------------------------------------" << std::endl;
   std::cout << "Line density " << std::endl;
   std::cout << "----------------------------------------------------------------------------------" << std::endl;

   int idx = 0;
   double sum = 0;
   for ( std::vector<double>::const_iterator it=monopole_.begin();  it != monopole_.end(); ++it, ++idx ) {

      std::cout << idx << "   " << *it << std::endl; 
      sum +=  (*it); 
  }
 
   std::cout << "----------------------------------------------------------------------------------" << std::endl;
   std::cout << "No of particles accounted for by summing slices (%) = " << sum          << std::endl;
   std::cout << "No of particles as reported by bunch.size()         = " << bunch.size() << std::endl;

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

double BunchProjector::cdt_min()
{
  return cdt_min_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

double BunchProjector::cdt_max()
{
  return cdt_max_;
}

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
