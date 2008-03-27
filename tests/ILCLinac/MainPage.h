/*! \mainpage 
*
*  This a partial reference manual for the ILC Linac test, which is part of a growing 
* CHEF test suite. 
*
* \section Introduction 
*
* The ''User's manual" consist of an informal technical note describing the 
* 4 related test.  All of them are based on the same Main Linac FODO lattice.
* This code tests various lattice manipulation and baseic tracking, for
* positrons  as well as Positron bunches (Wakefield effect included )  
* Please contact Chef managers for this user document. 
*
* \section Namespace and code organization;
*     The namespace Steering is used throughout this application. It contains 
*  a collection of free functions that implement various steering algorithms and
* a small collection of classes dedicated to the analysis of the performance 
* in terms of vertical emittance preservation. 
*
* \section StRel20080125 Upgrade 
* (i) Interface change to beamline element MoveRelX, MoveRelx,.. pitch, which
* now only accept a BmlPtr argument instead of a point to beamline element.  
* (ii) fix a few missing explicit namespace std:: using cout, cerr.... 
* (iii) fix accessing default data directory. 
* (iv) Test 3 now runs on the short lattice, to speed up testing procedure. 
*  Easy to revert to the long one, look for boolean shortLattice in the code. 
* 
* \section Disclaimer
* 
* This documentation is only partial: Since these tests have been derived from
* actual running code, used to do various benchmarks and analysis, 
* many methods are now obsolete and can be ignored. However, methods used at the
* top level code (ILCLinacTestx.cc, where x=1,2,3,4) are documented.
* 
*  Paul Lebrun, 2006-2007, finalized Feb 2008. 
*/
