/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******                                    
******  File:      LatticeFunctions.cc
******                                                                
******  Copyright Fermi Research Alliance / Fermilab    
******            All Rights Reserved                             
*****
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
****** 
******  Author: Jean-Francois Ostiguy 
******          ostiguy@fnal.gov                                                              
******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/

#include <beamline/LatticeFunctions.h>

// the dispersion is stored in a vector of dimension 6 for compatibility with 6D transfer matrices

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

dispersion_t::dispersion_t( )
 : eta(6), x( eta[0]),   xp( eta[3]),  y( eta[1]),  yp( eta[4]) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

dispersion_t::dispersion_t( dispersion_t const& o )
: eta(o.eta), x( eta[0]), xp( eta[3]),  y( eta[1]), yp( eta[4]) 
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

dispersion_t& dispersion_t::operator=( dispersion_t const& rhs )
{
  if ( this == &rhs ) return *this;

  eta = rhs.eta;
  return *this;
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs::CSLattFuncs()
  : arcLength(0.0), orbit(), dispersion(), beta(), alpha(),  psi()  
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs::CSLattFuncs( CSLattFuncs const& o)
  : arcLength(o.arcLength), orbit(o.orbit), 
    dispersion(o.dispersion), beta(o.beta), alpha(o.alpha),  psi(o.psi)  
{}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CSLattFuncs::defined() const 
{
  return ( ( beta.hor  ==  alpha.hor == 0.0 ) ||
           ( beta.ver  ==  alpha.ver == 0.0 ) ); 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CSLattFuncs4D::CSLattFuncs4D()
  : arcLength(0.0), orbit(), dispersion() 
{
  mode1.beta.hor  = 0.0;
  mode1.alpha.hor = 0.0;

  mode1.beta.ver  = 0.0; 
  mode1.alpha.ver = 0.0;

  mode1.psi       = 0.0; 

  mode2.beta.hor  = 0.0;
  mode2.alpha.hor = 0.0;

  mode2.beta.ver  = 0.0; 
  mode2.alpha.ver = 0.0;

  mode2.psi       = 0.0; 
  
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//
//  beta_1x = mode1.beta.hor
//  beta_1y = mode1.beta.ver

//  beta_2x = mode2.beta.hor
//  beta_2y = mode2.beta.ver

// etc ....

CSLattFuncs4D::CSLattFuncs4D( CSLattFuncs const& o):
     arcLength(o.arcLength),  
  orbit(o.orbit), 
    dispersion(o.dispersion)  
{

  mode1.beta.hor  = o.beta.hor;
  mode1.alpha.hor = o.alpha.hor;

  mode1.beta.ver  = 0.0; 
  mode1.alpha.ver = 0.0;

  mode1.psi       = o.psi.hor; 

  mode2.beta.hor  = 0.0;
  mode2.alpha.hor = 0.0;

  mode2.beta.ver  = o.beta.ver; 
  mode2.alpha.ver = o.alpha.ver;

  mode2.psi       = o.psi.ver; 

}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

bool CSLattFuncs4D::defined() const
{
  return ( ( mode1.beta.hor  ==  mode1.alpha.hor ==  mode1.beta.ver == mode1.alpha.ver == 0.0 ) ||
           ( mode2.beta.hor  ==  mode2.alpha.hor ==  mode2.beta.ver == mode2.alpha.ver == 0.0 ) ); 

}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ETLattFuncs::ETLattFuncs( CSLattFuncs const& o) 
  :   arcLength(o.arcLength),
   orbit(o.orbit), 
      dispersion(o.dispersion)
{  
   beta.hor  = o.beta.hor;
   alpha.hor = o.alpha.hor;

   psi.hor = o.psi.hor; 

   beta.ver  = o.beta.ver;
   alpha.ver = o.alpha.ver;

   psi.ver = o.psi.ver; 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

CVLattFuncs::CVLattFuncs( CSLattFuncs const& o)
  :   arcLength(o.arcLength),
      orbit(o.orbit), 
      dispersion(o.dispersion)
{  
   beta.hor  = o.beta.hor;
   alpha.hor = o.alpha.hor;

   psi.hor = o.psi.hor; 

   beta.ver  = o.beta.ver;
   alpha.ver = o.alpha.ver;

   psi.ver = o.psi.ver; 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#if 0 

#define PREC setprecision(4)

ostream& operator<<(ostream& os, LattFuncs const& x) {
  
  CSLattFuncs const& lf = boost::get<CSLattFuncs const&>(x);

  os << PREC << lf.alpha.hor << " " << lf.beta.hor << " " << lf.psi.hor;
  os << " " << lf.alpha.ver << " " << lf.beta.ver << " " << lf.psi.ver;
  os << " " << lf.dispersion.hor << " " << lf.dPrime.hor;
  os << " " << lf.dispersion.ver << " " << lf.dPrime.ver;
  return (os << endl);
}

istream& operator>>(istream& is, LattFuncs& x) {

  CSLattFuncs& lf = boost::get<CSLattFuncs&>(x);

  is >> lf.alpha.hor;
  is >> lf.beta.hor; 
  is >> lf.psi.hor ;
  is >> lf.alpha.ver; 
  is >> lf.beta.ver; 
  is >> lf.psi.ver; 
  is >> lf.dispersion.hor;
  is >> lf.dPrime.hor;
  is >> lf.dispersion.ver;
  is >> lf.dPrime.ver;
  return is;
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

ostream& operator<<(ostream& os, LattRing const& x) {
  os << x.tune.hor << " " << x.tune.ver << " ";
  os << " " << x.chromaticity.hor << " " << x.chromaticity.ver;
  return (os << endl);
}


//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||


#endif
