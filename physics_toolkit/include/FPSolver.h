#ifndef FPSOLVER_H
#define FPSOLVER_H

typedef char (*FP_CRITFUNC)( bmlnElmnt* );

struct FPinfo : BarnacleData {
 FPinfo( const double&, const Vector& );
 ~FPinfo() {}
 double arclength;
 Vector state;
};

struct FPSolver {
 private:
  int       dimension;
  double*   jumpScale;
  double*   zeroScale;
  int*      l;
  beamline* bmLine;
 public:
  FPSolver( beamline* = 0,  /* beamline attached to the solver */
            int       = 6   /* dimension of phase space        */ 
          );
  ~FPSolver();
  
  double& JumpScale( int ); 
  double& ZeroScale( int );

  int  operator()( Proton*,    const char*, FP_CRITFUNC = 0 );
  int  operator()( JetProton*, const char*, FP_CRITFUNC = 0 );   
                                                   // TransverseFixedPoint
  int  operator()( Proton*, FP_CRITFUNC = 0 );     // FixedPoint
  void operator()( JetProton*, FP_CRITFUNC = 0 );  // FixedPoint
  void operator()( double* );                      // fixedPoint
  // If default value is used for FP_CRITFUNC argument, then no 
  // fixed point information is attached to the elements.

 void eraseAll();
};

#endif // FPSOLVER_H
