/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:      ResonanceLine.h
******                                                                
******  Copyright (c) Fermi Research Alliance LLC
******                All Rights Reserved
******
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******  U.S. Department of Energy Contract No. DE-AC02-07CH11359.
******  The U.S. Government retains a world-wide non-exclusive, 
******  royalty-free license to publish or reproduce documentation 
******  and software for U.S. Government purposes. This software 
******  is protected under the U.S. and Foreign Copyright Laws. 
******
******                                                                
******  Author:    Jean-Francois Ostiguy                                     
******             ostiguy@fnal.gov                         
****** 
**************************************************************************
**************************************************************************
*************************************************************************/

#ifndef RESONANCELINE_H
#define RESONANCELINE_H

#include <qpoint.h>

class TuneDiagram::ResonanceLine : public QwtData {
 
 public: 

  ResonanceLine( int nx, int ny, int n ); 
  ResonanceLine(  ResonanceLine const& o); 
 ~ResonanceLine();

  ResonanceLine* copy () const;

  size_t         size ()         const;
  double  	 x (size_t i)    const;
  double 	 y (size_t i)    const;

  // QwtDoubleRect  boundingRect ();

 private:
  
  double nx_;
  double ny_;
  double n_;

  QPoint point1_;
  QPoint point2_;


};

#endif // RESONANCELINE_H
