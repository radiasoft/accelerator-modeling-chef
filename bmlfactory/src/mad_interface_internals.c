 /* -*- C -*- */

#include <math.h>

#if !defined(mad_interface_internals_h)
#include "mad_interface_internals.h"
#endif /* mad_interface_internals_h */

void
variable_init( variable* var ) {
   memset( var->name_, 0, VAR_NAME_MAX );
   var->value_ = HUGE;
}

void
variable_destroy( variable* var ) {
  variable_init( var );
}

void
constant_init( constant* cons ) {
   memset( cons->name_, 0, VAR_NAME_MAX );
   cons->value_ = HUGE;
}

void
constant_destroy( constant* cons ) {
  constant_init( cons );
}
