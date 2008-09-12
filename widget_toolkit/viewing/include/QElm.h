/*************************************************************************
**************************************************************************
**************************************************************************
******                                                                
******  CHEF: Library of Qt based widget classes, providing GUI   
******             interfaces to exercise the functionality        
******             of BEAMLINE.                                    
******                                                                
******  File:      QElm.h
******                                                                
******                                                              
**************************************************************************
*************************************************************************/

#ifndef QELM_H
#define QELM_H

#include <qlistview.h>
#include <beamline/BmlPtr.h>
#include <beamline/ElmPtr.h>


class QElm : public QListViewItem {

public:

  QElm( ElmPtr, double,  QElm*,      char const*, char const* );
  QElm( ElmPtr, double,  QListView*, char const*, char const* );
 ~QElm();

  void setOpen( bool o );
  void setup();
  void okRename( int col );

  QElm*       root_node();  

  QString fullName() const;

  ElmPtr          elm(); 
  
  static QPixmap const&      driftSymbol();
  static QPixmap const&       slotSymbol();
  static QPixmap const&   bmlBlackSymbol();
  static QPixmap const&   bmrBlackSymbol();
  static QPixmap const&  sextupoleSymbol();
  static QPixmap const&      fquadSymbol();
  static QPixmap const&      dquadSymbol();
  static QPixmap const&  bmlOrangeSymbol();
  static QPixmap const&  bmrOrangeSymbol();
  static QPixmap const&     dipoleSymbol();
  static QPixmap const&       lcavSymbol();
  static QPixmap const&      elmntSymbol();

 private:

  QElm( QElm const& );

  ElmPtr    elm_;
  double    azimuth_;   


};

#endif // ELM_H

