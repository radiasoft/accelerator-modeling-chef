#ifndef BunchPredicate_H
#define BunchPredicate_H

class BunchPredicate {
  public:
  
    //
    // Ctors & Dtors
    //
  
    inline   BunchPredicate( void ) {}
    virtual ~BunchPredicate( void ) {}

    //
    // Observer
    //

    virtual int operator()( const Particle& ) const {
      return 1;
    }
};

#endif // BunchPredicate_H
