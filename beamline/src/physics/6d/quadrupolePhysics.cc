#include "beamline.h"

quadrupole::TPOT_Prop::TPOT_Prop( int m )
: _n(m)
{
};

int quadrupole::TPOT_Prop::get_n() const
{
  return _n;
};

void quadrupole::TPOT_Prop::set_n( int m )
{
  _n = m;
};


int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, Particle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  return 0;
}

int quadrupole::TPOT_Prop::operator()( bmlnElmnt* pbe, JetParticle& p ) {
  ((quadrupole*) pbe)->p_bml->propagate( p );
  return 0;
}


void quadrupole::TPOT_Prop::setup( quadrupole* pqd ) const
{
  double xm   = _n;
  double lng  = pqd->Length();
  double str  = pqd->Strength();
 
  double frontLength     = 6.0*(lng/4.0)/15.0;
  double sepLength       = ( lng - 2.0*frontLength ) / 3.0;
  double quarterStrength = pqd->Strength()*lng/4.0;
 
  pqd->p_bml = new beamline;
  
  if( _n == 1 ) {           // One thin element ..........
    pqd->p_bml->append( new drift( lng / 2.0 ) );
    pqd->p_bml->append( pqd->p_bml_e = new thinQuad( str*lng ) );
    pqd->p_bml->append( new drift( lng / 2.0 ) );
  }
  
  else if( _n == 4 ) {      // TEAPOT-like schema .........
    pqd->p_bml->append( new drift( frontLength ) );
    pqd->p_bml->append( new thinQuad( quarterStrength ) );
    for( int i = 0; i < 3; i++ ) {
      pqd->p_bml->append( new drift( sepLength ) );
      pqd->p_bml->append( new thinQuad( quarterStrength ) );
    }
    pqd->p_bml->append( new drift( frontLength ) );
  }
  
  else {                   // Equal spacing ...............
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
    pqd->p_bml->append( new thinQuad( str*lng/ xm ) );
    for( int i = 0; i < _n-1; i++ ) {
      pqd->p_bml->append( new drift( lng / xm ) );
      pqd->p_bml->append( new thinQuad( str*lng/ xm ) );
    }
    pqd->p_bml->append( new drift( lng / ( 2.0*xm ) ) );
  }
}



quadrupole::TPOT_Prop  quadrupole::LikeTPOT(4);
