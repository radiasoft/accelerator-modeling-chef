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
**************************************************************************
*************************************************************************/


#ifndef SAGE_H
#include <Sage.h>
#endif

#ifndef VECTOR_H
#include <VectorD.h>
#endif

class beamline;

class SurveyMatcher : public Sage
{
 public:
  SurveyMatcher( const std::vector<Vector>&, beamline* );
  SurveyMatcher( const std::vector<Vector>&, beamline& );
  ~SurveyMatcher();

  static void removeCentroid( std::vector<Vector>&, Frame& );
  static void orientPlane( std::vector<Vector>&, Frame& );

  void eraseAll();
  void rotate();

  Frame getModelFrame() const;
  Frame getDataFrame() const;

  alignment getAlignment( int ) const;
  Vector    getLocalDisplacement( int ) const;

  int size() const;
  void print( std::vector<Vector>& ) const;

 private:
  std::vector<Vector> _surveyData;
  std::vector<Vector> _bufferData;
  std::vector<Vector> _modelCoordinates;
  std::vector<Vector> _inputModel;

  Frame _centralModelFrame;
  Frame _centralDataFrame;

  void _finishConstructor();

  void _diff();
  void _generateAlignments();
};


inline Frame SurveyMatcher::getModelFrame() const
{
  return _centralModelFrame;
}


inline Frame SurveyMatcher::getDataFrame() const
{
  return _centralDataFrame;
}


inline int SurveyMatcher::size() const
{
  return _modelCoordinates.size();
}


inline Vector SurveyMatcher::getLocalDisplacement( int i ) const
{
  return _modelCoordinates[i];
}
