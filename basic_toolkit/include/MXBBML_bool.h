#ifndef MXBBMLBOOL_H
#define MXBBMLBOOL_H

// The following code is for the Solaris native compiler.  It does not
// have any boolean basic data type.  Note that this does *not* make
// bool a basic type, but it does make some code more readable
// -- Elliott McCrory


// #if defined(__SUNPRO_CC)
#if defined(__SUNPRO_CC)
typedef enum { false = 0, true = 1 } bool;
#endif // __SUNPRO_CC

#endif // MXBBMLBOOL_H
