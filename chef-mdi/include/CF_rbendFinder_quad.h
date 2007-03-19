/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      CF_rbend_Finder_quad.h                            ****** 
******                                                               ******
******  Copyright (c) Universities Research Association, Inc.        ****** 
******                All Rights Reserved                            ****** 
******                                                               ****** 
******  Authors:                                                     ******
******              Leo Michelotti                                   ******
******              Fermilab                                         ******
******              michelotti@fnal.gov                              ****** 
******                                                               ******
******  Usage, modification, and redistribution are subject to terms ******
******  of the License supplied with this software.                  ****** 
******                                                               ******
******  Software and documentation created under                     ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.    ****** 
******  The U.S. Government retains a world-wide non-exclusive,      ****** 
******  royalty-free license to publish or reproduce documentation   ****** 
******  and software for U.S. Government purposes. This software     ****** 
******  is protected under the U.S. and Foreign Copyright Laws.      ****** 
******  URA/FNAL reserves all rights.                                ****** 
******                                                               ******
****** NOTE: Contents moved from CHEFGUI.cpp                         ******
***************************************************************************
***************************************************************************
**************************************************************************/

#ifndef CFRBENDFINDERQUAD_H
#define CFRBENDFINDERQUAD_H

#include <beamline/beamline.h>

class CF_rbendFinder_quad : public beamline::Criterion {

  public:
    
    CF_rbendFinder_quad( double = 0.0 );
   ~CF_rbendFinder_quad();

    void setQuadrupole( double );

    bool operator()( bmlnElmnt const& );

  private:

    double integratedQuad_;
};

#endif //  CFRBENDFINDERQUAD_H


