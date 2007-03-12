/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  Mxyzptlk:  A C++ implementation of differential algebra.      
******                                    
******  File:      BmlPtr.h
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
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
******  Author:    Jean-Francois Ostiguy
******             Email: ostiguy@fnal.gov                         
******
******                                                                
**************************************************************************
*************************************************************************/
#ifndef BMLPTR_H
#define BMLPTR_H

#include <complex>
#include <boost/shared_ptr.hpp>
#include <basic_toolkit/globaldefs.h>

class beamline;
class bmlnElmnt;
class BeamlineContext;

typedef boost::shared_ptr<beamline>               BmlPtr;
typedef boost::shared_ptr<const beamline>         ConstBmlPtr;

typedef boost::shared_ptr<bmlnElmnt>              ElmPtr;
typedef boost::shared_ptr<const bmlnElmnt>        ConstElmPtr;

typedef boost::shared_ptr<BeamlineContext>        BmlContextPtr;
typedef boost::shared_ptr<const BeamlineContext>  ConstBmlContextPtr;

#endif //  BMLPTR_H
