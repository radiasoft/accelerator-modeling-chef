#ifndef BMLFACTORY_H
#define BMLFACTORY_H

#include <functional>
#include <glib.h>
#include <list>

   // from MXYZPLTK
#include <beamline.h>

using namespace std;

#if !defined(beam_element_h)
#include "beam_element.h"
#endif /* beam_element.h */

#if !defined(beam_line_h)
#include "beam_line.h"
#endif /* beam_line.h */

#if !defined(madparser_h)
#include "madparser.h"
#endif /* madparser_h */

typedef char* char_ptr;

typedef struct {
    beam_element* bel_ptr_;  /* beam element from MAD parser */
    bmlnElmnt*    lbel_ptr_; /* beam element from Leo */
} bel_pair;

typedef list<bel_pair> bel_pair_list__t;

typedef struct {
    beam_line* bml_ptr_;  /* beam line from MAD parser */
    beamline*  lbml_ptr_; /* beam line from Leo */
} bml_pair;

typedef list<bml_pair> bml_pair_list__t;

struct bel_predicate : public binary_function<bel_pair,char_ptr,bool> {
    bool operator()( const bel_pair& x, const char_ptr& ptr ) const {
      char label[BEL_NAME_LENGTH];
      beam_element_get_label( x.bel_ptr_, label );
      if ( strcasecmp( label, ptr ) == 0 )
        return true;
      else
        return false;
    }
};

struct bml_predicate : public binary_function<bml_pair,char_ptr,bool> {
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
