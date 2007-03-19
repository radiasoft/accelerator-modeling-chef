/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      MAD8Factory.h
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
******  Authors:   Dmitri Mokhov, Oleg Krivosheev
******             Jean-Francois Ostiguy      
******                                                                
******  Contact:   Leo Michelotti or Jean-Francois Ostiguy            
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******                    (630) 840 2231                              
******             Email: michelotti@fnal.gov                         
******                    ostiguy@fnal.gov                            
******                                                                
****** REVISION HISTORY
****** Mar 2007   ostiguy@fnal.gov
****** - added support for reference counted elements/beamlines
**************************************************************************
*************************************************************************/

#ifndef MAD8FACTORY_H
#define MAD8FACTORY_H

#include <boost/shared_ptr.hpp>
#include <map>
#include <list>
#include <string>
#include <glib.h>
#include <bmlfactory/madparser.h>
#include <bmlfactory/beam_element.h>
#include <bmlfactory/beam_line.h>
#include <bmlfactory/bmlfactory.h>

class MAD8Factory: public bmlfactory {

  public:

    MAD8Factory( std::string fname, double brho, const char* stringbuffer=0); // deprecated
    MAD8Factory( std::string fname,              const char* stringbuffer=0);

   ~MAD8Factory();

    std::list<std::string>   getBeamlineList();       // list of all instantiated beamlines  

    char const* getUseStatementBeamlineName() const;   // get beamline name from USE statement, 0 if none found 
    char const* getParticleType() const;

    BmlPtr create_beamline( std::string bmlname,  double brho );
    BmlPtr create_beamline( std::string bmlname );               // deprecated 

    bool    variableIsDefined(const char* varname) const;
    double  getVariableValue(const char* varname) const;   

    double getEnergy() const;
    double getBrho() const;


  private:

    ElmPtr beam_element_instantiate(  beam_element* bel );
    ElmPtr find_beam_element( char const* name) const;

    //void create_bel_list();
    //void delete_bel_list();
    
    BmlPtr beam_line_instantiate( beam_line* );
    BmlPtr find_beam_line( char const* ) const;

    void append_bml_element( char*, beamline* );
    //void create_bml_list();
    //void delete_bml_list();

    BmlPtr create_beamline_private( const char*  beamline, double brho);    
    BmlPtr create_beamline_private( const char*  beamline);    


    void bmlfactory_init(const char* stringbuffer);

    std::string strip_final_colon( const char* );

    double                  BRHO_;
    double                  energy_;
    std::string             particle_type_name_;

    madparser*              mp_;

    GHashTable*             var_table_;
    GHashTable*             bel_table_;
    beam_element**          bel_arr_;
    int                     bel_arr_size_;
    beam_line**             bml_arr_;
    int                     bml_arr_size_;

    std::map<std::string,BmlPtr>  bml_map_;
    std::map<std::string,ElmPtr>  bel_map_;

};

#endif // MAD8BMLFACTORY_H
