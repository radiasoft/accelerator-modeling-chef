/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.
******                                    
******  File:      TJetWriter.h
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

#ifndef TJETWRITER_H
#define TJETWRITER_H

#include <string>
#include <basic_toolkit/globaldefs.h>
#include <mxyzptlk/TJet.h>
#include <mxyzptlk/TJetVector.h>


// Forward declarations
// --------------------
// template <typename T>
// class TJet;
// template <typename T>
// class TJetVector;


// Class definition
// ----------------
template <typename T>
class TJetWriter
{
 public:
  // Allowed styles and language
  enum Style { table = 0, equation, eqnarray, dirac };
  enum Lingo { text  = 0, LaTeX };

  // Constructors
  TJetWriter( );

  std::string write(   const TJet<T>&
                     , const std::vector<std::string>&
                     , const std::string& ) const;
  std::string write(   const TJetVector<T>&
                     , const std::vector<std::string>& ) const;

  // Modifiers
  Style  getStyle();
  Lingo  getLingo();
  void   setStyle( Style );
  void   setLingo( Lingo );

 private:
  Lingo  _lingo;
  Style  _style;

  std::string _writeLatexDiracTabular(   const TJet<T>&
                                       , const std::vector<std::string>&
                                       , const std::string& ) const;
  std::string _writeLatexDiracTabular(   const TJetVector<T>&
                                       , const std::vector<std::string>& ) const;
};

#endif // TJETWRITER_H
