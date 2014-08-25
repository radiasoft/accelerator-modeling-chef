/*************************************************************************
**************************************************************************
**************************************************************************
******
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and
******             synchrotrons.
******
******  File:      Dipedge.h
******
******  Copyright Universities Research Association, Inc., Fermilab
******            All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms
******  of the License supplied with this software.
******
******  Software and documentation created under
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.
******  The U.S. Government retains a world-wide non-exclusive,
******  royalty-free license to publish or reproduce documentation
******  and software for U.S. Government purposes. This software
******  is protected under the U.S. and Foreign Copyright Laws.
******
******  Author:    Leo Michelotti
******             Phone: (630) 840 4956
******             Email: michelotti@fnal.gov
******
******
****** REVISION HISTORY
****** ----------------
******
****** Apr 2013           Leo Michelotti
******                    michelotti@fnal.gov
****** - initial implementation, complete with errors.
******   THIS IS ONLY A PROTOTYPE VERSION, little
******   more than a placeholder file. 
****** - class Dipedge is meant to mimic the DIPEDGE element
******   used by MAD-X.
****** - Associated files:
******     beamline/include/DipedgePropagators.h
******     beamline/src/Dipedge.cc
******     beamline/src/DipedgePropagators.cc
******
**************************************************************************
*************************************************************************/

#ifndef DIPEDGE_H
#define DIPEDGE_H

#include <basic_toolkit/globaldefs.h>  // ??? OBSOLETE ???
#include <beamline/bmlnElmnt.h>


class BmlVisitor;
class ConstBmlVisitor;
class Dipedge;

typedef boost::shared_ptr<Dipedge> DipedgePtr; 
typedef boost::shared_ptr<Dipedge const> ConstDipedgePtr; 

class DLLEXPORT Dipedge : public bmlnElmnt
{

 public:

  class Propagator;

  typedef boost::shared_ptr<BasePropagator<Dipedge> > PropagatorPtr;

  // Ctors
  // -----
  Dipedge();
  Dipedge( char const* name, double const& length, double const& strength );
  Dipedge( char const* name, double const& length, double const& strength,
           double const& h, 
           double const& e1,
           double const& fint,
           double const& hgap,
           double const& tilt );
  Dipedge( Dipedge const& );

  Dipedge* Clone() const;

  ~Dipedge();


  // Visitor methods
  // ---------------
  void accept( BmlVisitor& );
  void accept( ConstBmlVisitor& ) const;


  // Modifiers
  // ---------
  void setStrength( double const& );


  // Propagators
  // -----------
  void localPropagate( Particle& );
  void localPropagate( JetParticle& );


  // Queries
  // -------
  bool isMagnet() const;
  inline const  char*  Type() const;

  inline double const& h()    const;
  inline double const& edge() const;
  inline double const& fint() const;
  inline double const& hgap() const;
  inline double const& tilt() const;
  inline double const& corr() const;
  inline bool          fsec() const;
  inline double        sig()  const;
  inline double        he()   const;
  inline double        sk1()  const;

  void usePropagator( PropagatorPtr& );

 private:
  double h_; 
  double edge_;
  double fint_;
  double hgap_;
  double tilt_;
  double corr_;

  PropagatorPtr propagator_;
};


// Inline functions
// ----------------
const char* Dipedge::Type() const { return "Dipedge"; }

double const& Dipedge::h()    const { return h_; }
double const& Dipedge::edge() const { return edge_; }
double const& Dipedge::fint() const { return fint_; }
double const& Dipedge::hgap() const { return hgap_; }
double const& Dipedge::tilt() const { return tilt_; }
double const& Dipedge::corr() const { return corr_; }
bool          Dipedge::fsec() const { return false; }  // ??? PLACEHOLDER: NEEDS TO BE FIXED ???
double        Dipedge::sig()  const { return 1.0;   }  // ??? PLACEHOLDER: NEEDS TO BE FIXED ???
double        Dipedge::he()   const { return 0.0;   }  // ??? PLACEHOLDER: NEEDS TO BE FIXED ???
double        Dipedge::sk1()  const { return 0.0;   }  // ??? PLACEHOLDER: NEEDS TO BE FIXED ???


#endif // DIPEDGE_H
