#include <stdlib.h>
#include "Particle.h"

#ifndef PARAMLIST_H
#include "ParameterList.h"
#endif

extern "C" int nearestInteger( double );

// ===
// === AccelParamAcc implementation ==========================
// ===

// ??? REMOVE void AccelParamAcc::alertConnectors( const AccelParam* p_ap )
// ??? REMOVE {
// ??? REMOVE   AccelParam* q;
// ??? REMOVE   slist_iterator getNext( p_ap->_connectors );
// ??? REMOVE   while((  q = (AccelParam*) getNext()  )) {
// ??? REMOVE     q->_ready = false;
// ??? REMOVE   }
// ??? REMOVE }


/* inline */ AccelParamAcc::AccelParamAcc( AccelParam* p, ParameterList* pl )
: _ap(p), _pl(pl)
{
}

/* inline */ AccelParamAcc::AccelParamAcc( const AccelParamAcc& x )
: _ap(x._ap), _pl(x._pl)
{
}

/* inline */ AccelParamAcc::~AccelParamAcc()
{
}

/* inline */ AccelParamAcc& AccelParamAcc::operator=( const double& x )
{
  if( _ap->_setValue( x ) ) {
    _ap->_ready = false;
  }
  else {
    _ap->_ready = true;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  return *this;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator*=( const double& x )
{
  if( _ap->_ready ) {
    _ap->_value *= x;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator*=( x );
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator*=                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return *this;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator/=( const double& x )
{
  if( x != 0.0 ) {
    if( _ap->_ready ) {
      _ap->_value /= x;
      // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
    }
  
    else {
      if( 0 == _ap->_recalc() ) {
  	_ap->_ready = true;
  	return this->operator/=( x );
      }
      else {
  	cerr << "*** ERROR ***                              \n"
  		"*** ERROR *** operator/=                   \n"
  		"*** ERROR ***                              \n"
  		"*** ERROR *** Error in recalculation.      \n"
  		"*** ERROR ***                              \n"
  	     << endl;
      }
    }
  }
  else {
    cerr << "*** ERROR ***                              \n"
            "*** ERROR *** operator/=                   \n"
            "*** ERROR ***                              \n"
            "*** ERROR *** Attempt to divide by zero.   \n"
            "*** ERROR ***                              \n"
         << endl;
    exit(1);
  }

  return *this;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator+=( const double& x )
{
  if( _ap->_ready ) {
    _ap->_value += x;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator+=( x );
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator+=                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return *this;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator-=( const double& x )
{
  if( _ap->_ready ) {
    _ap->_value += x;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator+=( x );
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator+=                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return *this;
}

/* inline */ double AccelParamAcc::operator-() const
{
  return -( _ap->_value );
}

/* inline */ double AccelParamAcc::operator+() const
{
  return _ap->_value;

}


/* inline */ AccelParamAcc& AccelParamAcc::operator=( const AccelParamAcc& x )
{
  return this->operator=( double(x) );
}

/* inline */ AccelParamAcc& AccelParamAcc::operator*=( const AccelParamAcc& x )
{
  return this->operator*=( double(x) );
}

/* inline */ AccelParamAcc& AccelParamAcc::operator/=( const AccelParamAcc& x )
{
  static double w;  // ??? Why this way?
  w = double(x);
  return this->operator/=( w );
}

/* inline */ AccelParamAcc& AccelParamAcc::operator+=( const AccelParamAcc& x )
{
  return this->operator+=( double(x) );
}

/* inline */ AccelParamAcc& AccelParamAcc::operator-=( const AccelParamAcc& x )
{
  return this->operator-=( double(x) );
}



/* inline */ double AccelParamAcc::operator++(int)
{
  double ret( 0.0 );
  if( _ap->_ready ) {
    ret = _ap->_value;
    (_ap->_value)++;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator++(0);
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator++                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return ret;
}

/* inline */ double AccelParamAcc::operator--(int)
{
  double ret( 0.0 );
  if( _ap->_ready ) {
    ret = _ap->_value;
    (_ap->_value)--;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator--(0);
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator--                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return ret;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator++()
{
  if( _ap->_ready ) {
    (_ap->_value)++;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator++();
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator++                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return *this;
}

/* inline */ AccelParamAcc& AccelParamAcc::operator--()
{
  if( _ap->_ready ) {
    (_ap->_value)--;
    // ??? REMOVE AccelParamAcc::alertConnectors( _ap );
  }

  else {
    if( 0 == _ap->_recalc() ) {
      _ap->_ready = true;
      return this->operator--();
    }
    else {
      cerr << "*** ERROR ***                              \n"
              "*** ERROR *** operator--                   \n"
              "*** ERROR ***                              \n"
              "*** ERROR *** Error in recalculation.      \n"
              "*** ERROR ***                              \n"
           << endl;
    }
  }

  return *this;
}

istream& operator>>( istream& is, AccelParamAcc x )
{
  static double u;
  is >> u;
  x._ap->_value = u;
  return is;
}

double AccelParamAcc::convert() const
{
  if( !(_ap->_ready) ) {
    _ap->_recalc();
  }
  return _ap->_value;
}



// ===
// === AccelParam implementation =============================
// ===

AccelParam::AccelParam( double v, bool r )
: _value(v), _ready(r), _tagged(false), _myList(0)
{
}

AccelParam::~AccelParam()
{
}


/* inline */ AccelParamAcc AccelParam::operator()()
{
  return AccelParamAcc( this, _myList );
}

// /* inline */ double AccelParam::operator()() const
// {
//   return _value;
//   // Note: _recalc cannot be done on a const object
// }


// ===
// === ParameterList implementation ==========================
// ===


ParameterList::ParameterList()
{
  _energy   ._myList = this;
  _psi      ._myList = this;
  _pf       ._myList = this;
  _Bfield   ._myList = this;
  _lQ       ._myList = this;
  _Carcs    ._myList = this;
  _Ccells   ._myList = this;
  _n        ._myList = this;
  _l        ._myList = this;
  _ef       ._myList = this;
  _grad     ._myList = this;
  _maxBetaH ._myList = this;
  _minBetaH ._myList = this;
  _maxDisp  ._myList = this;
  _minDisp  ._myList = this;
  _natChrom ._myList = this;
  _comp     ._myList = this;
}


ParameterList::ParameterList( const ParameterList& x )
: _energy   (x._energy),
  _psi      (x._psi),
  _pf       (x._pf),
  _Bfield   (x._Bfield),
  _lQ       (x._lQ),
  _Carcs    (x._Carcs),
  _Ccells   (x._Ccells),
  _n        (x._n),
  _l        (x._l),
  _ef       (x._ef),
  _grad     (x._grad),
  _maxBetaH (x._maxBetaH),
  _minBetaH (x._minBetaH),
  _maxDisp  (x._maxDisp),
  _minDisp  (x._minDisp),
  _natChrom (x._natChrom),
  _comp     (x._comp)
{
  _energy   ._myList = this;
  _psi      ._myList = this;
  _pf       ._myList = this;
  _Bfield   ._myList = this;
  _lQ       ._myList = this;
  _Carcs    ._myList = this;
  _Ccells   ._myList = this;
  _n        ._myList = this;
  _l        ._myList = this;
  _ef       ._myList = this;
  _grad     ._myList = this;
  _maxBetaH ._myList = this;
  _minBetaH ._myList = this;
  _maxDisp  ._myList = this;
  _minDisp  ._myList = this;
  _natChrom ._myList = this;
  _comp     ._myList = this;
}

ParameterList::~ParameterList()
{
}


ostream& operator<<( ostream& os, ParameterList& pl )
{
  cout << "Energy [GeV]                        : "
       << pl._energy()
       << endl;
  cout << "Phase advance  [degrees]            : "
       << pl._psi()*180.0/M_PI
       << endl;
  cout << "Packing factor                      : "
       << pl._pf()
       << endl;
  cout << "Dipole field [T]                    : "
       << pl._Bfield()
       << endl;
  cout << "Quadrupole length [m]               : "
       << pl._lQ()
       << endl;
  cout << "Perimeter of dipoles [km]           : " 
       << pl._Carcs()/1000.0
       << endl;
  cout << "Perimeter of standard cells [km]    : " 
       << pl._Ccells()/1000.0
       << endl;
  cout << "Number of cells                     : " 
       << pl._n()
       << endl;
  cout << "Cell length [m]                     : " 
       << pl._l()
       << endl;
  const double feet( 100.0 / (12.*2.54) );
         // Note: conversion factor from meters to feet
  cout << "Edwards' factor                     : " 
       << pl._ef()
       << endl;
  cout << "Quadrupole gradient [T/m]           : " 
       << pl._grad()
       << endl;
  cout << "Maximum beta [m]                    : " 
       << pl._maxBetaH()
       << endl;
  cout << "Minimum beta [m]                    : " 
       << pl._minBetaH()
       << endl;
  cout << "Maximum dispersion [m]              : " 
       << pl._maxDisp()
       << endl;
  cout << "Minimum dispersion [m]              : " 
       << pl._minDisp()
       << endl;
  cout << "Natural chromaticity                : " 
       << pl._natChrom()
       << endl;
  cout << "Momentum compaction (approx)        : " 
       << pl._comp()
       << endl;

  return os;
}



// ===
// === _recalc functions =====================================
// ===

int AccelParam::_setValue( double v )
{
  _value = v;
  return 0;
}


int ParameterList::Energy::_recalc()
{
  cout << "Not written" << endl;
  return 1;
};
  
int ParameterList::Energy::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  else {
    return 1;
  }
};
  
int ParameterList::Psi::_recalc()
{
  cout << "Not written" << endl;
  return 1;
};
  
int ParameterList::Psi::_setValue( double v )
{
  _value = v;
  while( M_TWOPI < _value ) _value -= M_TWOPI;
  while( _value  < 0.0    ) _value += M_TWOPI;
  return 0;
};
  
int ParameterList::Pack::_recalc()
{
  cout << "Not written" << endl;
  return 1;
};
  
int ParameterList::Pack::_setValue( double v )
{
  if( 0.0 < v && v < 1.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::BendField::_recalc()
{
  cout << "Not written" << endl;
  return 1;
};
  
int ParameterList::QuadLength::_recalc()
{
  cout << "Not written" << endl;
  return 1;
};
  
int ParameterList::QuadLength::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::ArcCircumference::_recalc()
{
  Proton xp( _myList->energy() );
  double brho( xp.ReferenceBRho() );
  double rho( brho / (double) _myList->Bfield() );

  _value = M_TWOPI*rho;
  // Note: C [km] = 20.958... * p[TeV/c] / B[T]

  return 0;
};
  
int ParameterList::ArcCircumference::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::CellCircumference::_recalc()
{
  _value = _myList->Carcs() / _myList->pf();
  return 0;
};
  
int ParameterList::CellCircumference::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::NumCells::_recalc()
{
  double psi( _myList->psi() );
  double maxD( _myList->maxDisp() );
  double C_cells( _myList->Ccells() );

  Proton xp( _myList->energy() );
  double brho( xp.ReferenceBRho() );
  double rho( brho / (double) _myList->Bfield() );

  double gfp, gfm, gfa, gfb;

  gfa = 1.0 / sin( psi/2. );
  gfb = gfa / 2.0;
  gfa = gfa*gfa;
  gfp = gfa + gfb;
  gfm = gfa - gfb;

  double L( sqrt(rho*maxD/gfp) );
         // Note: L is the half-cell length

  _value = C_cells / (2.*L );
  _value = (double) nearestInteger( _value );

  // Recalculate maxD assuming C_cells is invariant
  L = C_cells / (2.*_value );
  _myList->maxDisp()  = gfp*L*L/rho;
  _myList->minDisp() = gfm*L*L/rho;

  return 0;
};
  
int ParameterList::NumCells::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::CellLength::_recalc()
{
  double C_cells( _myList->Ccells() );
  double N( _myList->n() );

  double L( ( C_cells / N ) / 2.0 );
  _value = 2.0*L;

  return 0;
};
  
int ParameterList::CellLength::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::EdwardsFactor::_recalc()
{
  const double feet( 100.0 / (12.*2.54) );
         // Note: conversion factor from meters to feet
  double N( _myList->n() );
  double C_cells( _myList->Ccells() );

  _value = N / sqrt( C_cells * feet );
         // Note: This is Edwards' rule, modified

  return 0;
};
  
int ParameterList::QuadGrad::_recalc()
{
  double psi   ( _myList->psi() );
  double maxD  ( _myList->maxDisp() );
  Proton xp    ( _myList->energy() );
  double brho  ( xp.ReferenceBRho() );
  double rho   ( brho / (double) _myList->Bfield() );

  double gfp, gfm, gfa, gfb;

  gfa = 1.0 / sin( psi/2. );
  gfb = gfa / 2.0;
  gfa = gfa*gfa;
  gfp = gfa + gfb;
  gfm = gfa - gfb;

  double L     ( sqrt(rho*maxD/gfp) );
         // Note: L is the half-cell length

  double lQ    ( _myList->lQ() );

  _value = ( ( 2. * sin( psi/2. ) * brho / L ) / lQ );

  return 0;
};
  
int ParameterList::MaxBetaH::_recalc()
{
  double psi   ( _myList->psi() );
  double maxD  ( _myList->maxDisp() );
  Proton xp    ( _myList->energy() );
  double brho  ( xp.ReferenceBRho() );
  double rho   ( brho / (double) _myList->Bfield() );

  double gfp, gfm, gfa, gfb;

  gfa = 1.0 / sin( psi/2. );
  gfb = gfa / 2.0;
  gfa = gfa*gfa;
  gfp = gfa + gfb;
  gfm = gfa - gfb;

  double L     ( sqrt(rho*maxD/gfp) );
         // Note: L is the half-cell length

  

  _value = (2.0*L)*( 1.0 + sin( psi/2.) ) / sin( psi );
  return 0;
};
  
int ParameterList::MaxBetaH::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::MinBetaH::_recalc()
{
  double psi   ( _myList->psi() );
  double maxD  ( _myList->maxDisp() );
  Proton xp    ( _myList->energy() );
  double brho  ( xp.ReferenceBRho() );
  double rho   ( brho / (double) _myList->Bfield() );

  double gfp, gfm, gfa, gfb;

  gfa = 1.0 / sin( psi/2. );
  gfb = gfa / 2.0;
  gfa = gfa*gfa;
  gfp = gfa + gfb;
  gfm = gfa - gfb;

  double L     ( sqrt(rho*maxD/gfp) );
         // Note: L is the half-cell length

  

  _value = (2.0*L)*( 1.0 - sin( psi/2.) ) / sin( psi );
  return 0;
};
  
int ParameterList::MinBetaH::_setValue( double v )
{
  if( v > 0.0 ) {
    _value = v;
    return 0;
  }
  return 1;
};
  
int ParameterList::MaxDisp::_recalc()
{
  // ??? cout << "Maximum dispersion [m]              : " 
  // ???      << gfp*L*L/rho
  // ???      << endl;

  return 1;
};
  
int ParameterList::MinDisp::_recalc()
{
  double psi   ( _myList->psi() );
  double maxD  ( _myList->maxDisp() );
  Proton xp    ( _myList->energy() );
  double brho  ( xp.ReferenceBRho() );
  double rho   ( brho / (double) _myList->Bfield() );

  double gfp, gfm, gfa, gfb;

  gfa = 1.0 / sin( psi/2. );
  gfb = gfa / 2.0;
  gfa = gfa*gfa;
  gfp = gfa + gfb;
  gfm = gfa - gfb;

  double L     ( sqrt(rho*maxD/gfp) );
         // Note: L is the half-cell length

  _value = gfm*L*L/rho;
  return 0;
};
  
int ParameterList::NaturalChromaticity::_recalc()
{
  double N    ( _myList->n()   );
  double psi  ( _myList->psi() );

  _value = - M_1_PI * tan( psi/2. ) * N;
  return 0;
};
  
int ParameterList::MomentumCompaction::_recalc()
{
  double N    ( _myList->n()   );
  double psi  ( _myList->psi() );

  double gfa = ( M_PI / sin( psi/2. ) ) / N;
  _value     = gfa*gfa;

  return 0;
};

