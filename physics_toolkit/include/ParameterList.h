#ifndef PARAMLIST_H
#define PARAMLIST_H

#include <iostream.h>

#include "MathConstants.h"      // To get bool
#include "slist.h"

class ParameterList;
class AccelParam;

class AccelParamAcc
{
 public:
  AccelParam*     _ap;
  ParameterList*  _pl;

  AccelParamAcc( AccelParam*, ParameterList* );
  AccelParamAcc( const AccelParamAcc& );
  ~AccelParamAcc();

  AccelParamAcc& operator= ( const double& );
  AccelParamAcc& operator*=( const double& );
  AccelParamAcc& operator/=( const double& );
  AccelParamAcc& operator+=( const double& );
  AccelParamAcc& operator-=( const double& );
  double operator-() const;
  double operator+() const;

  // The following are necessary to allow for statements
  // like x(i,j) += y(i,j) without ambiguity.
  AccelParamAcc& operator= ( const AccelParamAcc& );
  AccelParamAcc& operator*=( const AccelParamAcc& );
  AccelParamAcc& operator/=( const AccelParamAcc& );
  AccelParamAcc& operator+=( const AccelParamAcc& );
  AccelParamAcc& operator-=( const AccelParamAcc& );

  double  operator++(int);          // postfix
  AccelParamAcc&    operator++();   // prefix
  double  operator--(int);          // postfix
  AccelParamAcc&    operator--();   // prefix

  double  convert() const;

  friend istream& operator>>( istream&, AccelParamAcc );
  operator double() const { return this->convert(); }

 private:
  // ??? REMOVE static void alertConnectors( const AccelParam* );
};



// ======================================================

class AccelParam
{
 public:
  AccelParam( double = 0.0, bool = false );
  ~AccelParam();

  AccelParamAcc operator()();
  // double        operator()() const;

 protected:
  double _value;          // Current value of parameter
  bool   _ready;          // Whether ready for use in computations
  bool   _tagged;         // Used by _recalc to avoid circularity
  // ??? REMOVE slist  _connectors;     // All parameters affected by current value
  ParameterList* _myList; // Parent parameter list

  virtual int _recalc() = 0;
  virtual int _setValue( double );
  
  void _unsetReady ( const AccelParamAcc& x )
    { x._ap->_ready = false; }
  void   _setReady ( const AccelParamAcc& x )
    { x._ap->_ready = true;  }  

  friend istream& operator>>( istream&, AccelParamAcc );
  friend ostream& operator<<( ostream&, ParameterList& );
  friend class AccelParamAcc;
  friend class ParameterList;
};


// ======================================================

class ParameterList
{
 public:
  ParameterList();
  ParameterList( const ParameterList& );
  ~ParameterList();

  friend ostream& operator<<( ostream&, ParameterList& );
  friend class AccelParamAcc;
  friend class AccelParam;

  AccelParamAcc energy() 
    { return _energy(); }
  // int set_energy( double x )
  //   { return _energy() = x; }
  
  AccelParamAcc psi() 
    { return _psi(); }
  // int set_psi( double x )
  //   { return _psi() = x; }
  
  AccelParamAcc pf() 
    { return _pf(); }
  // int set_pf( double x )
  //   { return _pf() = x; }
  
  AccelParamAcc Bfield() 
    { return _Bfield(); }
  // int set_Bfield( double x )
  //   { return _Bfield() = x; }
  
  AccelParamAcc lQ() 
    { return _lQ(); }
  // int set_lQ( double x )
  //   { return _lQ() = x; }
  
  AccelParamAcc Carcs() 
    { return _Carcs(); }
  // int set_Carcs( double x )
  //   { return _Carcs() = x; }
  
  AccelParamAcc Ccells() 
    { return _Ccells(); }
  // int set_Ccells( double x )
  //   { return _Ccells() = x; }
  
  AccelParamAcc n() 
    { return _n(); }
  // int set_n( double x )
  //   { return _n() = x; }
  
  AccelParamAcc l() 
    { return _l(); }
  // int set_l( double x )
  //   { return _l() = x; }
  
  AccelParamAcc ef() 
    { return _ef(); }
  // int set_ef( double x )
  //   { return _ef() = x; }
  
  AccelParamAcc grad() 
    { return _grad(); }
  // int set_grad( double x )
  //   { return _grad() = x; }
  
  AccelParamAcc maxBetaH() 
    { return _maxBetaH(); }
  // int set_maxBetaH( double x )
  //   { return _maxBetaH() = x; }
  
  AccelParamAcc minBetaH() 
    { return _minBetaH(); }
  // int set_minBetaH( double x )
  //   { return _minBetaH() = x; }
  
  AccelParamAcc maxDisp() 
    { return _maxDisp(); }
  // int set_maxDisp( double x )
  //   { return _maxDisp() = x; }
  
  AccelParamAcc minDisp() 
    { return _minDisp(); }
  // int set_minDisp( double x )
  //   { return _minDisp() = x; }
  
  AccelParamAcc natChrom() 
    { return _natChrom(); }
  // int set_natChrom( double x )
  //   { return _natChrom() = x; }
  
  AccelParamAcc comp() 
    { return _comp(); }
  // int set_comp( double x )
  //   { return _comp() = x; }


 private:
  struct Energy : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  Energy  _energy;                // [GeV] design energy

  struct Psi : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  Psi     _psi;                   // [rad] phase advance per cell

  struct Pack : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  Pack _pf;                       //       packing fraction  

  struct BendField : public AccelParam
  {
    int _recalc();
  };
  BendField _Bfield;              // [T]   bend field

  struct QuadLength : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  QuadLength _lQ;                 // [m]   quad length in cell

  struct ArcCircumference : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  ArcCircumference _Carcs;        // [m]   perimeter of dipoles

  struct CellCircumference : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  CellCircumference _Ccells;      // [m]   perimeter of standard cells

  struct NumCells : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  NumCells _n;                    //       number of standard cells

  struct CellLength : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  CellLength _l;                  // [m]   cell length

  struct EdwardsFactor : public AccelParam
  {
    int _recalc();
  };
  EdwardsFactor _ef;              //       Edwards factor

  struct QuadGrad : public AccelParam
  {
    int _recalc();
  };
  QuadGrad _grad;                 // [T/m] quadrupole gradient

  struct MaxBetaH : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  MaxBetaH _maxBetaH;             // [m]   maximum horizontal beta in cell

  struct MinBetaH : public AccelParam
  {
    int _recalc();
    int _setValue( double );
  };
  MinBetaH _minBetaH;             // [m]   minimum horizontal beta

  struct MaxDisp : public AccelParam
  {
    int _recalc();
  };
  MaxDisp _maxDisp;               // [m]   maximum dispersion

  struct MinDisp : public AccelParam
  {
    int _recalc();
  };
  MinDisp _minDisp;               // [m]   minimum dispersion

  struct NaturalChromaticity : public AccelParam
  {
    int _recalc();
  };
  NaturalChromaticity _natChrom;  //       natural chromaticity

  struct MomentumCompaction : public AccelParam
  {
    int _recalc();
  };
  MomentumCompaction _comp;       //       momentum compaction (approx)


};

#endif // PARAMLIST_H
