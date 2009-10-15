/********************************************************************************
*********************************************************************************
**
** From the boost:signals FAQ
**
** How do I get Boost.Signals to work with Qt?
**
** When building with Qt, the Moc keywords signals and slots are defined using 
** preprocessor macros, causing programs using Boost.Signals and Qt together to 
** fail to compile.
**
** For Qt 4.1 and later, This behavior can be turned off in Qt on a per-project 
** or per-file basis with the no_keywords option. This works with out-of-the-box 
** builds of Boost and Qt. You do not need to re-configure, re-build, or 
** duplicate existing libraries. For a project where you want to use both 
** Boost.Signals and Qt Signals and Slots, the relevant part of your .pro file 
** might look like this:
**
** CONFIG      += no_keywords # so Qt won't #define any non-all-caps `keywords'
** INCLUDEPATH += . /usr/local/include/boost-1_33_1/
** macx:LIBS   += /usr/local/lib/libboost_signals-1_33_1.a  # 
** ...your exact paths may vary
** Now you can mix Boost.Signals and Qt Signals and Slots in the same files, 
** and even within the same class or function. 
** You will have** to use the upper-case versions of Qt macros in your own code. 
** See the article A Deeper Look at Signals and Slots [off-site] 
** for more complete examples and a survey of the strengths of the two systems.
**
** Older versions of Qt did not provide a reliable mechanism for avoiding these 
** unfriendly, all lower-case `keyword'-like macros. Although this is a problem 
** with Qt and not Boost.Signals, a user can use the two systems together with 
** a little extra effort. There are two ways to do this:
**
** The first way involves defining the BOOST_SIGNALS_NAMESPACE macro to some 
** other identifier (e.g., signalslib) when building and using the 
** Boost.Signals library. Then the namespace of the Boost.Signals 
** library will be boost::BOOST_SIGNALS_NAMESPACE instead of boost::signals. 
** To retain the original namespace name in translation units that do not 
** interact with Qt, you can use a namespace alias:
**
**  namespace boost {
**    namespace signals = BOOST_SIGNALS_NAMESPACE;
**  }
** 
** The second way, provided by Frank Hess and improved by Niels Dekker, 
** involves creating a wrapper header file for boost::signal. 
**
** +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
** This wrapper is meant to be included _in lieu_ of <boost/signal.hpp>
** +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
**
********************************************************************************
********************************************************************************/

#ifndef SIGNALSLIB_HPP_INCLUDED
#define SIGNALSLIB_HPP_INCLUDED 

#if defined(signals) && defined(QOBJECTDEFS_H) && defined(QT_MOC_CPP)
#  undef signals
#  define signals signals
#endif

#include <boost/signal.hpp>
namespace boost
{
  namespace signalslib = signals;
}

#if defined(signals) && defined(QOBJECTDEFS_H) && !defined(QT_MOC_CPP)
#  undef signals
// Restore the macro definition of "signals", as it was
// defined by Qt's <qobjectdefs.h>.
#  define signals protected
#endif

#endif //  SIGNALSLIB_HPP_INCLUDE

