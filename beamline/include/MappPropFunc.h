#ifndef MAPPPROPFUNC_H
#define MAPPPROPFUNC_H

#include "beamline.h"

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

  protected:
    Mapping _myMap;
};

#endif
