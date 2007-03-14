/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  PHYSICS TOOLKIT: Library of utilites and Sage classes         
******             which facilitate calculations with the             
******             BEAMLINE class library.                            
******                                    
******  File: SurveyMatcher.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.    
******                All Rights Reserved                             
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
******  Author:    Leo Michelotti                                     
******
******             Fermilab                                           
******             P.O.Box 500                                        
******             Mail Stop 220                                      
******             Batavia, IL   60510                                
******                                                                
******             Phone: (630) 840 4956                              
******             Email: michelotti@fnal.gov                         
******                                                                
****** Revision History:
******
******   September, 2005
******     First version
******
****** Mar 2007           ostiguy@fnal.gov
****** - eliminated refereces to slist/dlist
******                                                   
**************************************************************************
*************************************************************************/


#include <boost/shared_ptr.hpp>
#include <physics_toolkit/Sage.h>
#include <basic_toolkit/globaldefs.h>
#include <basic_toolkit/VectorD.h>
#include <beamline/Alignment.h>
#include <beamline/BmlPtr.h>

class SurveyMatcher : public Sage {

 public:

  SurveyMatcher( std::vector<Vector> const&, BmlPtr );
 
 ~SurveyMatcher();

  static void removeCentroid( std::vector<Vector>&, Frame& );
  static void orientPlane(    std::vector<Vector>&, Frame& );

  void eraseAll();
  void rotate();

  Frame getModelFrame() const;
  Frame getDataFrame()  const;

  alignment getAlignment( int )         const;
  Vector    getLocalDisplacement( int ) const;

  int  size()                        const;
  void print( std::vector<Vector>& ) const;

 private:

  void  finishConstructor();

  void  diff();
  void  generateAlignments();

  std::vector<Vector>  surveyData_;
  std::vector<Vector>  bufferData_;
  std::vector<Vector>  modelCoordinates_;
  std::vector<Vector>  inputModel_;

  Frame                centralModelFrame_;
  Frame                centralDataFrame_;


};


