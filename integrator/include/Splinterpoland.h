#ifndef SPLINTERPOLAND_H
#define SPLINTERPOLAND_H

#include "AbsIntegrator.h"

#include <vector>

class Splinterpoland : public AbsIntegrator<double>::Integrand
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

    Splinterpoland();
    ~Splinterpoland();
    double operator()( const double& );
    double derivative( const double& );
    int acceptNode( double, double );
    void setLeftDerivative( double );
    void setRightDerivative( double );
    void setBoundaryDerivatives( double, double );

  private:
    std::vector<Node> _nodes;
    void _sort();
    void _wipeClean();
    void _build();
    MatrixD* _M;
    MatrixD* _A;
    MatrixD* _d;
    int      _n;    // number of nodes in the mesh
    double   _spl;  // derivative at left boundary
    double   _spr;  // derivative at right boundary
};


#endif  // SPLINTERPOLAND_H
