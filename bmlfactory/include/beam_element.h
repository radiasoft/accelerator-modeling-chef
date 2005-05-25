/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  BEAMLINE FACTORY:  Interprets MAD input files and             
******             creates instances of class beamline.                       
******                                                
******  Version:   1.2
******                                    
******  File:      beam_element.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******                                                                
******  Author:    Dmitri Mokhov and Oleg Krivosheev                  
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
******                                                                
**************************************************************************
*************************************************************************/


   /* -*- C -*- */

#ifndef beam_element_h
#define beam_element_h

#include <glib.h>

#if !defined(fb_allocator_h)
#include "fb_allocator.h"
#endif /* fb_allocator.h */

#ifdef __cplusplus
extern "C" {
#endif

/* From "The MAD Program Version 8.13/8 User's Reference Manual" */

#define BEL_UNKNOWN     (0)
#define BEL_MARKER      (1<<0)
#define BEL_DRIFT       (1<<1)
#define BEL_SBEND       (1<<2)
#define BEL_RBEND       (1<<3)
#define BEL_QUADRUPOLE  (1<<4)
#define BEL_SEXTUPOLE   (1<<5)
#define BEL_OCTUPOLE    (1<<6)
#define BEL_MULTIPOLE   (1<<7)
#define BEL_SOLENOID    (1<<8)
#define BEL_HKICKER     (1<<9)
#define BEL_VKICKER     (1<<10)
#define BEL_KICKER      (1<<11)
#define BEL_RFCAVITY    (1<<12)
#define BEL_ELSEPARATOR (1<<13)
#define BEL_HMONITOR    (1<<14)
#define BEL_VMONITOR    (1<<15)
#define BEL_MONITOR     (1<<16)
#define BEL_INSTRUMENT  (1<<17)
#define BEL_ECOLLIMATOR (1<<18)
#define BEL_RCOLLIMATOR (1<<19)
#define BEL_YROT        (1<<20)
#define BEL_SROT        (1<<21)
#define BEL_BEAMBEAM    (1<<22)
#define BEL_MATRIX      (1<<23)
#define BEL_LUMP        (1<<24)
#define BEL_BEAMLINE    (1<<31)

#define BEL_SBEND_ANGLE       0
#define BEL_SBEND_K1          1
#define BEL_SBEND_E1          2
#define BEL_SBEND_E2          3
#define BEL_SBEND_TILT        4
#define BEL_SBEND_K2          5
#define BEL_SBEND_H1          6
#define BEL_SBEND_H2          7
#define BEL_SBEND_HGAP        8
#define BEL_SBEND_FINT        9
#define BEL_SBEND_K3         10

#define BEL_RBEND_ANGLE       0
#define BEL_RBEND_K1          1
#define BEL_RBEND_E1          2
#define BEL_RBEND_E2          3
#define BEL_RBEND_TILT        4
#define BEL_RBEND_K2          5
#define BEL_RBEND_H1          6
#define BEL_RBEND_H2          7
#define BEL_RBEND_HGAP        8
#define BEL_RBEND_FINT        9
#define BEL_RBEND_K3         10

#define BEL_QUADRUPOLE_K1     0
#define BEL_QUADRUPOLE_TILT   1

#define BEL_SEXTUPOLE_K2      0
#define BEL_SEXTUPOLE_TILT    1

#define BEL_OCTUPOLE_K3       0
#define BEL_OCTUPOLE_TILT     1

#define BEL_MULTIPOLE_K0L     0
#define BEL_MULTIPOLE_T0      1
#define BEL_MULTIPOLE_K1L     2
#define BEL_MULTIPOLE_T1      3
#define BEL_MULTIPOLE_K2L     4
#define BEL_MULTIPOLE_T2      5
#define BEL_MULTIPOLE_K3L     6
#define BEL_MULTIPOLE_T3      7
#define BEL_MULTIPOLE_K4L     8
#define BEL_MULTIPOLE_T4      9
#define BEL_MULTIPOLE_K5L    10
#define BEL_MULTIPOLE_T5     11
#define BEL_MULTIPOLE_K6L    12
#define BEL_MULTIPOLE_T6     13
#define BEL_MULTIPOLE_K7L    14
#define BEL_MULTIPOLE_T7     15
#define BEL_MULTIPOLE_K8L    16
#define BEL_MULTIPOLE_T8     17
#define BEL_MULTIPOLE_K9L    18
#define BEL_MULTIPOLE_T9     19

#define BEL_SOLENOID_KS       0

#define BEL_HKICKER_KICK      0
#define BEL_HKICKER_TILT      1

#define BEL_VKICKER_KICK      0
#define BEL_VKICKER_TILT      1

#define BEL_KICKER_HKICK      0
#define BEL_KICKER_VKICK      1
#define BEL_KICKER_TILT       2

#define BEL_RFCAVITY_VOLT     0
#define BEL_RFCAVITY_LAG      1
#define BEL_RFCAVITY_HARMON   2
#define BEL_RFCAVITY_BETRF    3
#define BEL_RFCAVITY_PG       4
#define BEL_RFCAVITY_SHUNT    5
#define BEL_RFCAVITY_TFILL    6

#define BEL_ELSEPARATOR_E     0
#define BEL_ELSEPARATOR_TILT  1

#define BEL_ECOLLIMATOR_XSIZE 0
#define BEL_ECOLLIMATOR_YSIZE 1

#define BEL_RCOLLIMATOR_XSIZE 0
#define BEL_RCOLLIMATOR_YSIZE 1

#define BEL_YROT_ANGLE        0

#define BEL_SROT_ANGLE        0

#define BEL_BEAMBEAM_SIGX     0
#define BEL_BEAMBEAM_SIGY     1
#define BEL_BEAMBEAM_XMA      2
#define BEL_BEAMBEAM_YMA      3
#define BEL_BEAMBEAM_CHARGE   4

#define BEL_LUMP_ORDER        0
#define BEL_LUMP_LINE         1

#define BEL_BEAMLINE_SIGN     0

#define BEL_NAME_LENGTH       128
#define BEL_NOF_PARAMS         24

typedef struct beam_element_ {
    int         kind_;
    int         linenum_;
    const char* filename_;
    int         local_linenum_;
    char        name_[BEL_NAME_LENGTH];
    GNode*      length_;
    GNode*      params_[BEL_NOF_PARAMS];
    double      reference_energy_;
    void*       udata_;
} beam_element;

typedef  struct {
    beam_element* bel;
    int           flag;
} bel_handler;

beam_element*  beam_element_init( fb_allocator* expr_alloc, fb_allocator* bel_alloc );

void           beam_element_set_kind( beam_element* bel, int kind );
void           beam_element_set_name( beam_element* bel, const char* name, const char* type );
void           beam_element_set_length( beam_element* bel, GNode* length, fb_allocator* expr_alloc );
void           beam_element_set_param( beam_element* bel, int param_idx, GNode* value );
void           beam_element_set_udata( beam_element* bel, void* udata );
  
void           beam_element_set_linenum( beam_element* bel, int linenum, const char* filename, int local_linenum );

int            beam_element_delete( beam_element* bel, fb_allocator* bel_alloc, fb_allocator* expr_alloc );

beam_element*  beam_element_copy( beam_element* bel, fb_allocator* bel_alloc );

void           beam_element_get_label( beam_element* bel, char* label );
void           beam_element_display( FILE* out, beam_element* bel, GHashTable* var_table, GHashTable* bel_table );

#ifdef __cplusplus
};
#endif

#endif /*beam_element_h */
