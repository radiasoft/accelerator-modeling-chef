/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE:  C++ objects for design and analysis
******             of beamlines, storage rings, and   
******             synchrotrons.                      
******  Version:   2.0                    
******                                    
******  File:      MappPropFunc.h
******                                                                
******  Copyright (c) 1991 Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef MAPPPROPFUNC_H
#define MAPPPROPFUNC_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
#endif

#ifndef MAP_HXX
#include "Mapping"
#endif

class MappPropFunc : public bmlnElmnt::PropFunc
{
  public:
    int operator()( bmlnElmnt*, Particle&    );
    int operator()( bmlnElmnt*, JetParticle& );
    const char* Type() const { return "MappPropFunc"; }

    MappPropFunc( JetParticle&, bmlnElmnt& );
    MappPropFunc( const Mapping& );
    MappPropFunc( const MappPropFunc& );
    ~MappPropFunc(){}

    void setMapping( const Mapping& );
    Mapping getMapping();

  protected:
    Mapping _myMap;
};

#endif
