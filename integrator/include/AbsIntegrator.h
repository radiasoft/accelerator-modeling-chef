#ifndef ABSINTEGRATOR_H
#define ABSINTEGRATOR_H

#include "VectorD.h"

template<class T>
class AbsIntegrator
{
public:

  class Integrand {
  public:
    // virtual T operator()( const T& ) const = 0;
    virtual T operator()( const T& ) = 0;
  };

  class Method {
  public:
    virtual T integrate( double /* lower bound: t_i */,
                         double /* upper bound: t_f */ ) const = 0;
    virtual void registerIntegrator( AbsIntegrator* );
  protected:
    AbsIntegrator* _integrator;
  };

  AbsIntegrator();
  virtual ~AbsIntegrator();

  void setNStep(int);
  int  getNStep();

  void setMethod( Method* );
  void setIntegrand( Integrand* );

  T integrate( double /* lower bound: t_i */, 
               double /* upper bound: t_f */ ) const;
  T getValue( const T& ) const;

protected:
  Method*       _method;     // not owned
  Integrand*    _integrand;  // not owned
  int           _dim;
  int           _nStep;
};


#include "AbsIntegrator.tcc"

#endif // ABSINTEGRATOR_H
