/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      DialogElmInfo.cc
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
*************************************************************************
*************************************************************************
************************************************************************/

#include <DialogElmInfo.h>
#include <qtable.h>


DialogElmInfo::DialogElmInfo( QWidget* parent, char const* name, bool modal, WFlags f) 
  : DialogElmInfoBase(parent, name, modal, f)
{

  table->setNumRows(0);
  table->setNumCols(3);

  table->horizontalHeader()->setLabel(0, "Property");
  table->horizontalHeader()->setLabel(1, "Unit(s)");
  table->horizontalHeader()->setLabel(2, "Value");
  table->verticalHeader()->hide();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

void DialogElmInfo::addPropertyRow( QString prop, QString units,  QString val )
{ 
  table->setNumRows( table->numRows() + 1); 
  table->setText( table->numRows()-1, 0, prop);
  table->setText( table->numRows()-1, 1, units);
  table->setText( table->numRows()-1, 2, val);
  table->adjustSize();
}

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
