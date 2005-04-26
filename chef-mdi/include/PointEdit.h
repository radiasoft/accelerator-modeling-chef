/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  Version:   3.0                    
******                                    
******  File:      PointEdit.h
******                                                                
******  Copyright (c) 2004  Universities Research Association, Inc.   
******                All Rights Reserved                             
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
******  Usage, modification, and redistribution are subject to terms          
******  of the License and the GNU General Public License, both of
******  which are supplied with this software.
******                                                                
**************************************************************************
*************************************************************************/


#ifndef POINTEDIT_H
#define POINTEDIT_H

#include <qlineedit.h>

class QHBox;

class PointEdit : public QLineEdit
{
Q_OBJECT

public:
  PointEdit( const char*, QHBox* );
  ~PointEdit();

public slots:
  void _set_first ( double, double );
  void _set_second( double, double );
  void _txtchg( const QString& );
  void _retprs();

signals:
  void _new_value( double );

};

#endif // POINTEDIT_H
