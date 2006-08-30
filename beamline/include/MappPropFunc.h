/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.1
******                                    
******  File:      MappPropFunc.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
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
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
******                                                                
**************************************************************************
*************************************************************************/

#ifndef MAPPPROPFUNC_H
#define MAPPPROPFUNC_H

#include <basic_toolkit/globaldefs.h>
#include <beamline/bmlnElmnt.h>
#include <mxyzptlk/Mapping.h>


class DLLEXPORT MappPropFunc : public bmlnElmnt::PropFunc
{
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "MappPropFunc"; }

    MappPropFunc( JetParticle&, bmlnElmnt& );
    MappPropFunc( const Mapping& );
    MappPropFunc( const MappPropFunc& );
    virtual ~MappPropFunc(){}

    void setMapping( const Mapping& );
    Mapping getMapping();

  protected:
    Mapping _myMap;
};

#endif
