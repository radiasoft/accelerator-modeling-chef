/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File:      TJetWriter.tcc
******                                                                
******  Copyright (c) 2006  Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Leo Michelotti                                     
******                                                                
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 234
******             Batavia, IL   60510
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
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
****** Revision History:
****** ----------------
******   May, 2006: 
******     - Still just a prototype but nonetheless useful.
******
**************************************************************************
*************************************************************************/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include <basic_toolkit/globaldefs.h>
#include <physics_toolkit/TJetWriter.h>

using namespace std;

template <typename T>
TJetWriter<T>::TJetWriter()
:   _style(TJetWriter::table)
  , _lingo(TJetWriter::LaTeX)
{
}


template <typename T>
string TJetWriter<T>::write(    const TJet<T>&                  jetRef
                              , const std::vector<std::string>& names 
                              , const string&                   ff     ) const
{
  if( TJetWriter::LaTeX == _lingo ) {
    if( TJetWriter::table == _style ) {
      return _writeLatexDiracTabular( jetRef, names, ff );
    }
  }
}


template <typename T>
string TJetWriter<T>::write(  const TJetVector<T>&            mapRef
                            , const std::vector<std::string>& names ) const
{
  if( TJetWriter::LaTeX == _lingo ) {
    if( TJetWriter::table == _style ) {
      return _writeLatexDiracTabular( mapRef, names );
    }
  }
}


template <typename T>
string TJetWriter<T>::TJetWriter::_writeLatexDiracTabular(   const TJet<T>&                  jetRef
                                                           , const std::vector<std::string>& names
                                                           , const string&                   ff     ) const
{
  stringstream uic;

  const TJLterm<T>* termPtr;
  int d = jetRef.getEnvNumVar();
  int e;
  IntArray exps(d);

  const_cast<TJet<T>&>(jetRef).resetConstIterator();
  termPtr = jetRef.stepConstIteratorPtr();
  while( 0 != termPtr ) {
    uic << "\\\\ $\\langle \\,";
    exps = termPtr->exponents();
    for( int j = 0; j < d; j++ ) {
      e = exps(j);
      if( 0 != e ) {
        if( 1 == e ) {
          uic << "\\," << names[j];
	}
        else {
          uic << "\\," << names[j] << "^{" << e << "}";
	}
      }        
    }
    uic << "$ & $|$ & $" 
        << ff 
        << " \\, \\rangle$ & $=$ & $" 
        << termPtr->coefficient() 
        << "$"
        << endl;
    termPtr = jetRef.stepConstIteratorPtr();
  }

  return uic.str();
}


template <typename T>
string TJetWriter<T>::TJetWriter::_writeLatexDiracTabular(   const TJetVector<T>&            mapRef
                                                           , const std::vector<std::string>& names ) const
{
  stringstream uic;

  int d = mapRef.Dim();
  uic << "\n\n\\begin{tabular}{rclcl}" << endl;
  for( int i = 0; i < d; i++ ) {
    uic << TJetWriter::_writeLatexDiracTabular( mapRef(i), names, names[i] );
  }
  uic << "\\end{tabular}" << endl;

  return uic.str();
}
