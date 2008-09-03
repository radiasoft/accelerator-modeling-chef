/*************************************************************************
**************************************************************************
**************************************************************************
******                                    
******  File:      BmlBrowser.h
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
#ifndef BMLBROWSER_H
#define BMLBROWSER_H

#include <beamline/ElmPtr.h>
#include <beamline/BmlPtr.h>
#include <beamline/beamline.h>
#include <beamline/BmlVisitor.h>
#include <qlistview.h>
#include <QElm.h>

class BoolNode;

class BmlBrowser :public QListView, public ConstBmlVisitor {

    Q_OBJECT

public:

  BmlBrowser( QWidget *parent = 0, const char *name = 0, WFlags f=0);
 ~BmlBrowser();
  
  void       displayBeamline( BmlPtr );
  void        removeBeamline( BmlPtr );   

  std::list<ElmPtr>      selectElements(  ElmPtr toplevel, BoolNode const&  query );
  std::list<ElmPtr>         getSelection()       const; 
  std::list<ElmPtr> getTopLevelSelection()       const; 
  std::list<ElmPtr>      getElmSelection()       const; 


public slots:

    void resetPixmap( ConstBmlPtr );

signals:

    void signalElmLeftClicked( QElm* );
    void signalBrowserIsEmpty();
    void signalBrowserIsNotEmpty();

private slots:

    void         slotShowData( QElm* );

private:

    void   contentsMousePressEvent( QMouseEvent *e );
    static void  displayLine( QElm*, BmlPtr, beamline::reverse_iterator rit, double& );

    void visit(bmlnElmnt     const&);
    void visit(beamline      const&);
    void visit(drift         const&);
    void visit(Slot          const&);
    void visit(sbend         const&);
    void visit(CF_sbend      const&);
    void visit(rbend         const&);
    void visit(CF_rbend      const&);
    void visit(quadrupole    const&);
    void visit(thinQuad      const&);
    void visit(sextupole     const&);
    void visit(marker        const&);
    void visit(sector        const&);
    void visit(monitor       const&);
    void visit(LinacCavity   const&);

    QPoint          presspos_;
    bool            mousePressed_;

};

#endif // BMLBROWSER_H
