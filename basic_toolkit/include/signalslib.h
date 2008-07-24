
/********************************************************************************
*********************************************************************************

  When building with Qt, the Moc keywords "signals" and "slots" are defined 
  using preprocessor macros, causing programs using Boost.Signals and Qt together 
  to fail to compile.

  For Qt 4.1 and later, this behavior can be turned off in Qt on a per-project 
  or per-file basis with the "no_keywords option". This works with out-of-the-box 
  builds of Boost and Qt. You do not need to re-configure, re-build, or duplicate 
  existing libraries. You can mix Boost.Signals and Qt Signals and Slots in the 
  same files, and even within the same class or function. 
  NOTE: You will have to use the **upper-case** versions of the Qt macros in 
        your own code. 
  See the article "A Deeper Look at Signals and Slots"for more complete examples 
  and a survey of the strengths of the two systems.

  Older versions of Qt do not provide a reliable mechanism for avoiding these 
  unfriendly, all lower-case `keyword'-like macros. Although this is a problem 
  with Qt and not Boost.Signals, a user can use the two systems together with 
  a little extra effort. This is the purpose of this wrapper, which is meant 
  to be included _in lieu_ of <boost/signal.hpp>

********************************************************************************
********************************************************************************/

#ifdef signals
#error "signalslib.hpp must be included before any qt header"
#endif

#ifndef SIGNALSLIB_H
#define SIGNALSLIB_H

#include <boost/signal.hpp>
namespace boost
{
  namespace signalslib = signals;
}

#endif
