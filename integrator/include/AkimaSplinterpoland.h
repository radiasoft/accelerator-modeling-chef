#ifndef AKIMASPLINTERPOLAND_H
#define AKIMASPLINTERPOLAND_H

#include "AbsIntegrator.h"

#include <vector>

extern "C" {
#include <gsl/gsl_interp.h> // Regrettably, must be included here.
#include <gsl/gsl_spline.h>
}

// class gsl_spline;        This doesn't work, because the tokens are defined
// class gsl_interp_accel;  with typedef statements in gsl header files.

class AkimaSplinterpoland : public AbsIntegrator<double>::Integrand
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

    AkimaSplinterpoland();
    ~AkimaSplinterpoland();
    double operator()( const double& );
    double derivative( const double& );
    int acceptNode( double, double );

  private:
    std::vector<Node> _nodes;
    gsl_spline* _spline;
    gsl_interp_accel* _accelerator;
    void   _sort();
    void   _wipeClean();
    void   _build();
    int    _n;    // number of nodes in the mesh
    double _firstX;
    double _lastX;
    double _firstY;
    double _lastY;
};


#endif  // AKIMASPLINTERPOLAND_H
