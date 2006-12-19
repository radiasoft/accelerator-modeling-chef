/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  XSIF Parser:  Interprets XSIF input files and             
******                creates instances of class beamline.                       
******                                                
******                                    
******  File:      XSIFFactory.h
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
******  Author:    Jean-Francois Ostiguy      
******             ostiguy@fnal.gov
******                                                                
******
**************************************************************************
*************************************************************************/

#ifndef XSIFFACTORY_H
#define XSIFFACTORY_H

#include <beamline/beamline.h>
#include <bmlfactory/bmlfactory.h>
#include <parsers/xsif/XsifParserDriver.h>


class XSIFFactory: public bmlfactory {

  public:

    XSIFFactory( std::string filename, double brho, const char* stringbuffer=0); 
    XSIFFactory( std::string filename, const char* stringbuffer=0);

   ~XSIFFactory();

    beamline* create_beamline( std::string beamline,  double brho);
    beamline* create_beamline( std::string beamline); 
        
    std::list<std::string>   getBeamlineList();          

    const char* getUseStatementBeamlineName() const;           

    const char* getParticleType() const;
    double getEnergy()            const;
    double getBrho()              const;

    bool    variableIsDefined(const char* varname) const;
    double  getVariableValue(const char* varname) const;   


  private:
 
    XsifParserDriver driver_;

};

#endif // BMLFACTORY_H
