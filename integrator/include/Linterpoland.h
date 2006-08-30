#ifndef LINTERPOLAND_H
#define LINTERPOLAND_H

#include <integrator/AbsIntegrator.h>

#include <vector>

class Linterpoland : public AbsIntegrator<double>::Integrand
{
  public: 

    struct Node {
      double _x;
      double _y;
      bool operator<( const Node& ) const;
      struct cmp
      {
        bool operator()( const Node&, const Node& ) const;
      };
    };

    Linterpoland();
    ~Linterpoland();
    double operator()( const double& );
    double derivative( const double& );
    int acceptNode( double, double );

    Linterpoland& operator=( const Linterpoland& );
    void removeDomain( double, double );
    void clear();

  private:
    std::vector<Node> _nodes;
    void _sort();
};


#endif  // LINTERPOLAND_H
