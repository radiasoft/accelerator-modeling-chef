 /* -*- C -*- */

#ifndef mad_interface_internals_h
#define mad_interface_internals_h

void variable_init( variable* );
void variable_destroy( variable* );

void constant_init( constant* );
void constant_destroy( constant* );

void beam_element_init( beam_element* );
void beam_element_destroy( beam_element* );

void beam_line_init( beam_line* );
void beam_line_destroy( beam_line* );

#endif /* mad_interface_internals_h */
