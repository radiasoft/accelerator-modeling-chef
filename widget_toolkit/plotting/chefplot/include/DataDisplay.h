////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                            //
// FILE:       datadisplay.h                                                                  //
//                                                                                            //
// AUTHOR(S):  Jean-Francois Ostiguy                                                          // 
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
//             Accelerator Division / Accelerator Integration Dept                            //
//             Fermi National Laboratory, Batavia, IL                                         //
//             ostiguy@fnal.gov                                                               //
//                                                                                            //
// DATE:       September 2004                                                                 //
//                                                                                            //
// COPYRIGHT: Universities Research Association                                               //
//                                                                                            //
//                                                                                            //
////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef DATADISPLAY_H
#define DATADISPLAY_H

#include "datadisplaybase.h"
#include <ostream>

class DataDisplay: public DataDisplayBase {
 
 Q_OBJECT

public:

 DataDisplay(QWidget* parent, const char* name, WFlags f, bool showbml=false);
~DataDisplay();

public slots:

 void filePrint();
 void fileSaveAs();
 void fileSave();
 void fileExit();
 void viewMerge(bool set);

 private:

 void output( std::ostream& os);
 bool showbml_;

};

#endif
