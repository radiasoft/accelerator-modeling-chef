#ifndef MAPPPROPFUNC_H
#define MAPPPROPFUNC_H

#ifndef BMLNELMNT_H
#include "bmlnElmnt.h"
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
