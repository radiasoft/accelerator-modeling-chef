template<class T>
AbsIntegrator<T>::AbsIntegrator()
: _dim(1), _nStep(32), _method(0), _integrand(0)
{
}


template<class T>
AbsIntegrator<T>::~AbsIntegrator()
{
}


template<class T>
void AbsIntegrator<T>::setNStep( int x )
{
  _nStep = x;
}


template<class T>
void AbsIntegrator<T>::setIntegrand( Integrand* p )
{
  if( 0 != _integrand ) {
    std::cerr << "\n*** WARNING *** Cannot assign two integrands to an integrator.\n" << std::endl;
  }
  else if( 0 == p ) {
    std::cerr << "\n*** WARNING *** Attempt to assign null pointer as integrand.\n" << std::endl;
  }
  else {
    _integrand = p;
  }
}


template<class T>
void AbsIntegrator<T>::setMethod( Method* p )
{
  if( 0 != _method ) {
    std::cerr << "\n*** WARNING *** Cannot assign two methods to an integrator.\n" << std::endl;
  }
  else if( 0 == p ) {
    std::cerr << "\n*** WARNING *** Attempt to assign null pointer as integrand.\n" << std::endl;
  }
  else {
    _method = p;
    p->registerIntegrator( this );
  }
}


template<class T>
T AbsIntegrator<T>::integrate( double t_i, double t_f ) const
{
  return _method->integrate( t_i, t_f );
}


template<class T>
T AbsIntegrator<T>::getValue( const T& x ) const
{
  return (*_integrand)(x);
}


template<class T>
int AbsIntegrator<T>::getNStep()
{
  return _nStep;
}


template<class T>
void AbsIntegrator<T>::Method::registerIntegrator( AbsIntegrator* x ) 
{
  _integrator = x;
}

