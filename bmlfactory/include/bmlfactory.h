/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******                                    
******  File:      bmlfactory.h
******                                                                
******  Copyright (c) Universities Research Association, Inc. / Fermilab  
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
******                                                                
******  Authors:   Jean-Francois Ostiguy      
******             ostiguy@fnal.gov                                                    
******
******  NOTE: bmlfactory is an abstract interface class. The old bmlfactory
******        class is now called MAD8BmlFactory.  
******                                                        
**************************************************************************
*************************************************************************/
#ifndef BMLFACTORY_H
#define BMLFACTORY_H

#include <list>
#include <string>

class beamline;

class bmlfactory {

  public:

    bmlfactory(  std::string fname,              const char* stringbuffer );
    bmlfactory(  std::string fname, double brho, const char* stringbuffer );

    virtual ~bmlfactory() = 0;

    virtual beamline* create_beamline( std::string bmlname ,  double brho) = 0;
    virtual beamline* create_beamline( std::string bmlname)                = 0;
        
    virtual std::list<std::string>  getBeamlineList() =0;          // list of all available beamlines  

    virtual const char* getUseStatementBeamlineName()   const =0; 
          
    virtual const char* getParticleType()               const =0;
    virtual double      getEnergy()                     const =0;
    virtual double      getBrho()                       const =0;

    virtual bool        variableIsDefined(const char* varname) const =0;
    virtual double      getVariableValue (const char* varname) const =0;   



  protected:

    std::string             fname_;


};

 inline bmlfactory::~bmlfactory() {}

#endif // BMLFACTORY_H
