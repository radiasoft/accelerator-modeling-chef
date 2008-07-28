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

//
//  beta_1x = mode1.beta.hor
//  beta_1y = mode1.beta.ver

//  beta_2x = mode2.beta.hor
//  beta_2y = mode2.beta.ver

// etc ....

CSLattFuncs4D::CSLattFuncs4D( CSLattFuncs const& o):
     arcLength(o.arcLength),  
  reference_orbit(o.reference_orbit), 
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

ETLattFuncs::ETLattFuncs( CSLattFuncs const& o) 
  :   arcLength(o.arcLength),
   reference_orbit(o.reference_orbit), 
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
   reference_orbit(o.reference_orbit), 
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
