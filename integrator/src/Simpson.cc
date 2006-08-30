#include <integrator/Simpson.h>

Simpson::Simpson()
{
}


double Simpson::integrate( double t_i, double t_f ) const
{
  if( t_f == t_i ) { return 0.0; }
  if( t_f < t_i  ) { return -(this->integrate( t_f, t_i )); }

  int i;
  int n = _integrator->getNStep();
  double dt = ( t_f - t_i ) / ((double) n);
  double f[n+1];
  double t = t_i;
  for( i = 0; i <=  n; i++ ) {
    f[i] = _integrator->getValue( t );
    t += dt;
  }

  double ret = 0.0;
  for( i = 1; i < n; i++ ) {
    ret += f[i];
  }
  ret += ( f[0] + f[n] )/2.0;
  ret *= dt;

  return ret;
}

