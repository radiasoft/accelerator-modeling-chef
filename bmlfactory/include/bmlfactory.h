/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
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
**************************************************************************
*************************************************************************/
#ifndef BMLFACTORY_H
#define BMLFACTORY_H

#include <functional>
#include <glib.h>
#include <list>
#include <string>

// from MXYZPLTK
#include <beamline.h>
#include <beam_element.h>
#include <beam_line.h>
#include <madparser.h>


typedef char* char_ptr;

typedef struct {
    beam_element* bel_ptr_;  /* beam element from MAD parser */
    bmlnElmnt*    lbel_ptr_; /* beam element from Leo */
} bel_pair;

typedef std::list<bel_pair> bel_pair_list__t;

typedef struct {
    beam_line* bml_ptr_;  /* beam line from MAD parser */
    beamline*  lbml_ptr_; /* beam line from Leo */
} bml_pair;

typedef std::list<bml_pair> bml_pair_list__t;

struct bel_predicate : public std::binary_function<bel_pair,char_ptr,bool> {
    bool operator()( const bel_pair& x, const char_ptr& ptr ) const {
      char label[BEL_NAME_LENGTH];
      beam_element_get_label( x.bel_ptr_, label );
      if ( strcasecmp( label, ptr ) == 0 )
        return true;
      else
        return false;
    }
};

struct bml_predicate : public std::binary_function<bml_pair,char_ptr,bool> {
    bool operator()( const bml_pair& x, const char_ptr& ptr ) const {
      if ( strcasecmp( (x.bml_ptr_)->name_, ptr ) == 0 )
        return true;
      else
        return false;
    }
};


class bmlfactory {

  private:

    char*                   fname_;
    double                  BRHO_;
    madparser*              mp_;
    GHashTable*             var_table_;
    GHashTable*             bel_table_;
    beam_element**          bel_arr_;
    int                     bel_arr_size_;
    beam_line**             bml_arr_;
    int                     bml_arr_size_;
    bel_pair_list__t*       bel_list_;
    bml_pair_list__t*       bml_list_;

    std::list<std::string> _beamline_identifiers_list;
    std::list<beamline*>   _beamline_objects_list;
    std::list<bmlnElmnt*>  _bmlnElmnt_objects_list;
 

    bmlnElmnt* beam_element_instantiate( beam_element* );
    bmlnElmnt* find_beam_element( char* );

    void create_bel_list();
    void delete_bel_list();
    
    beamline* beam_line_instantiate( beam_line* );
    beamline* find_beam_line( char* );

    void append_bml_element( char*, beamline* );
    void create_bml_list();
    void delete_bml_list();

    beamline* create_beamline_private( const char*  beamline, double brho);    
    beamline* create_beamline_private( const char*  beamline);    

    void bmlfactory_init(const char* fname, const char* stringbuffer);

    std::string strip_final_colon( const char* );

  public:

    bmlfactory( const char* filename, double brho, const char* stringbuffer=0); // deprecated
    bmlfactory( const char* filename, const char* stringbuffer=0);

    ~bmlfactory();

    beamline* create_beamline( const char* beamline,  double brho);
    beamline* create_beamline( const char* beamline); // deprecated 
        
    std::list<std::string>&  getBeamlineList();       // list of all instantiated beamlines  
    const char* getUseStatementBeamlineName();        // get beamline name from USE statement, 0 if none found 
    const char* getParticleType() const;

    bool    variableIsDefined(const char* varname) const;
    double  getVariableValue(const char* varname) const;   

    double getEnergy() const;
    double getBrho() const;
};

#endif // BMLFACTORY_H
