/*
 * Distribtion class written by Jim Holt.
 * August 11, 1994
 *
 */

#ifndef DISTRIBUTION_HXX
#define DISTRIBUTION_HXX

extern "C" void   srand48( long );
extern "C" double drand48( void );

class Distribution {
protected:
  long initialSeed;
public:
  Distribution(long seed) {initialSeed = seed; srand48(seed);}
  virtual ~Distribution() { }
  virtual double getValue() { return drand48();}
};

class Gaussian : public Distribution {
public:
  Gaussian(long);
  virtual ~Gaussian();
  virtual double getValue();
};

#endif  // DISTRIBUTION_HXX
