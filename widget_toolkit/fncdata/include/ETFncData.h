/*******************************************************************************
********************************************************************************
********************************************************************************
******                                                                    ******   
******  CHEF:      An application layered on the Beamline/mxyzptlk        ****** 
******             class libraries.                                       ****** 
******                                                                    ****** 
******  File:      ETFncData.h                                            ****** 
******                                                                    ******
******  Copyright (c) Universities Research Association, Inc.             ****** 
******                All Rights Reserved                                 ****** 
******                                                                    ****** 
******  Authors:                                                          ******
******                                                                    ******
******              Jean-Francois Ostiguy                                 ******
******              Fermilab                                              ****** 
******              ostiguy@fnal.gov                                      ****** 
******                                                                    ******  
******              Leo Michelotti                                        ******
******              Fermilab                                              ******
******              michelotti@fnal.gov                                   ****** 
******                                                                    ******
******  Usage, modification, and redistribution are subject to terms      ******
******  of the License supplied with this software.                       ****** 
******                                                                    ******
******  Software and documentation created under                          ****** 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000.         ****** 
******  The U.S. Government retains a world-wide non-exclusive,           ****** 
******  royalty-free license to publish or reproduce documentation        ****** 
******  and software for U.S. Government purposes. This software          ****** 
******  is protected under the U.S. and Foreign Copyright Laws.           ****** 
******  URA/FNAL reserves all rights.                                     ****** 
******                                                                    ******
********************************************************************************
********************************************************************************
********************************************************************************/

#ifndef ETFNCDATA_H
#define ETFNCDATA_H

#include <beamline/BmlPtr.h>
#include <physics_toolkit/EdwardsTengSage.h>
#include <PlotData.h>


class ETFncData: public PlotData {

 public:

    ETFncData( std::vector<EdwardsTengSage::Info> const& et_vec, ConstBmlPtr bml = ConstBmlPtr() );
   ~ETFncData();

};

#endif // ETFNCPLT_H
