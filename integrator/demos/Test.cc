#include <basic_toolkit/iosetup.h>
#include <basic_toolkit/VectorD.h>
#include <integrator/RungeKutta.h>
#include <integrator/ODE.h>

using namespace std;


// ******************************************
// class Oscillator
// ******************************************
class Oscillator : public ODE 
{
  private:
  public:
    Oscillator();
    ~Oscillator();
    Vector operator() (const Vector&, double);
};


Oscillator::Oscillator() 
: ODE() 
{
  dimen = 2;
}


Oscillator::~Oscillator() 
{
}


Vector Oscillator::operator() (const Vector& state, double /* t */ )
{
  Vector ret(dimen);
  if (dimen == 2){
    ret(0) =   state(1);
    ret(1) = - state(0);
  }
  else{
    cerr << "Oscillator::operator(): Wrong dimension." << std::endl;
  }
  return ret;
}


// ******************************************
// Main program
// ******************************************

main()
{
  RungeKutta rk;
  rk.setNStep( 10 );

  Oscillator xp;

  Vector s(2);
  s(0) = 1.;
  s(1) = 0.;
  
  Vector s1(2);
  
  cout.precision(16);
  cout << "Start integration:" << endl;
  s1 = rk.integrate( 1, xp, xp, s, 0, 4*atan(1.0) );
  cout << s1 << endl;
  cout << "Symplecticity error = " << 1-(s1(0)*s1(0)+s1(1)*s1(1)) << endl;
  cout << "End integration:" << endl;

  rk.setNStep( 10 );
  cout << "s(0)" << s(0) << endl;
  cout << "s(1)" << s(1) << endl;
  cout << "Start integration:" << endl;
  s1 = rk.integrate( 2, xp, xp, s, 0, 4*atan(1.0), 1e-2, .4e-9 );
  cout << s1 << endl;
  cout << "Symplecticity error = " << 1-(s1(0)*s1(0)+s1(1)*s1(1)) << endl;
  cout << "End integration:" << endl;
}
