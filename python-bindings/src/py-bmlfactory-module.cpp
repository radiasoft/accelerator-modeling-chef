/***************************************************************************                                                               
******  Boost.python Python bindings for mxyzpltk/beamline libraries 
******  
******                                    
******  File:      py-bmlfactory-module.cpp
******                                                                
******  Copyright (c) Universities Research Association, Inc./ Fermilab    
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S.and Foreign Copyright Laws. 
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******                                                                
******             Fermi National Laboratory, Batavia, IL   60510                                
******             ostiguy@fnal.gov                         
******
****************************************************************************/
#include <boost/python.hpp>
#include <bmlfactory/bmlfactory.h>

using namespace boost::python;

// #define BOOST_PYTHON_STATIC_MODULE


beamline* ( bmlfactory::* create_beamline_1 )(const char*        )        = &bmlfactory::create_beamline;
beamline* ( bmlfactory::* create_beamline_2 )(const char*, double)        = &bmlfactory::create_beamline;

BOOST_PYTHON_MODULE(bmlfactory)
{

class_<bmlfactory>("bmlfactory", init<const char*, const char*>() )
  .def( init<const char*, double, const char*>() ) 
  .def( init<const char*>() ) 
  .def( init<const char*, double>() ) 
  .def("create_beamline",  create_beamline_1, return_value_policy<manage_new_object>() )
  .def("create_beamline",  create_beamline_2, return_value_policy<manage_new_object>() );
}


/////////////////////////////////////////////////////////////////////////////////////////
#if 0

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
    char*             fname_;
    double            BRHO_;
    madparser*        mp_;
    GHashTable*       var_table_;
    GHashTable*       bel_table_;
    beam_element**    bel_arr_;
    int               bel_arr_size_;
    beam_line**       bml_arr_;
    int               bml_arr_size_;
    bel_pair_list__t* bel_list_;
    bml_pair_list__t* bml_list_;
    
    bmlnElmnt* beam_element_instantiate( beam_element* );
    bmlnElmnt* find_beam_element( char* );
    void create_bel_list();
    void delete_bel_list();
    
    beamline* beam_line_instantiate( beam_line* );
    beamline* find_beam_line( char* );
    void append_bml_element( char*, beamline* );
    void create_bml_list();
    void delete_bml_list();
    
  public:
    bmlfactory( const char* /* filename */,
                double /* BRHO */ );

    ~bmlfactory();

    beamline* create_beamline( const char*  /* beamline name */ );
};

#endif // BMLFACTORY_H
