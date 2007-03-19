/**************************************************************************
***************************************************************************
***************************************************************************
******                                                               ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk   ****** 
******             class libraries.                                  ****** 
******                                                               ****** 
******  File:      RollAccumulator.h                                 ****** 
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

#ifndef ROLLACCUMULATOR_H
#define ROLLACCUMULATOR_H

#include <beamline/beamline.h>

class RollAccumulator : public beamline::Action {

  public:
    RollAccumulator( const char*, double = 0.0 );
    ~RollAccumulator();

    void setWeakType();
    void setStrongType();

    int operator()( bmlnElmnt& );

  private:
    double deltaRoll_;
    char*  type_;
    bool   weak_;   // default false
};

#endif // ROLLACCUMULATOR_H
