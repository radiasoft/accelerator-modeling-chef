/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF:      A Qt-based Application Framework
******             using the BEAMLINE class library.
******                                                                
******  File:      DbRetrieveDialog.h
******                                                                
******  Copyright (c) Universities Research Association, Inc.   
******                All Rights Reserved                             
******                                                                
******  Author:    Jean-Francois Ostiguy
******                                                                
******             Fermilab, Batavia, IL                                
******             ostiguy@fnal.gov                         
******                                                                
******                                                                
******  Usage, modification, and redistribution are subject to terms          
******  of the License supplied with this software.
******  
******  Software and documentation created under 
******* U.S. Department of Energy Contract No. DE-AC02-76CH03000. 
******* The U.S. Government retains a world-wide non-exclusive, 
******* royalty-free license to publish or reproduce documentation 
******* and software for U.S. Government purposes. This software 
******* is protected under the U.S. and Foreign Copyright Laws. 
******* URA/FNAL reserves all rights.
*******                                                                
**************************************************************************
**************************************************************************
*************************************************************************/
#ifndef DBRETRIEVEDIALOG_H
#define DBRETRIEVEDIALOG_H

#include <dbretrievedialogbase.h>
#include <qstring.h>
#include <qvariant.h>
#include <vector>


typedef std::vector<QVariant> Many;

struct column_item { 
    const char* name;
    const char* label;
};

class QListBoxItem;

class DbRetrieveDialog: public DbRetrieveDialogBase {

Q_OBJECT

 public:

 DbRetrieveDialog(QWidget* parent, const char* name, WFlags f=0);
 ~DbRetrieveDialog();

 private slots:

   void _handleButtonOkClicked();
   void _handleSurveyTypeCheckBoxToggled(bool set);
   void _handleListBoxMachinesClicked(QListBoxItem* item);
   void _handleDipoleBendsChecked(bool set);
   void _handleMainQuadsChecked(bool set);
   void _handleCurrentTabChanged( QWidget* w); 
   void _handleAllMomentsChecked(bool set); 
   void _handleSeriesChecked(bool set);
   void _handleSeriesSelectionChanged();

 private:

   virtual void showEvent (QShowEvent * );

   void     _refreshMachineSelection( ); 
   void     _refreshSeriesSelection(QString s);
   void     _refreshSurveyDataSelection(const QString& machine );
   void     _displayLocationsInfo();
   void     _displayMomentsInfo();
   void     _displaySurveyXYHInfo();
   void     _displaySurveyXYZInfo();
   void     _displayDataTable( QString query_string, Many query_params, column_item col_info[] );
   QString  _getSelectedSeriesList();


};

#endif
