/****************************************************************************
** Form interface generated from reading ui file 'chefplotmainbase.ui'
**
** Created: Wed Jul 7 16:19:24 2004
**      by: The User Interface Compiler ($Id: chefplotmainbase.h,v 1.2 2004/07/16 21:46:14 michelot Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef CHEFPLOTMAINBASE_H
#define CHEFPLOTMAINBASE_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;

class CHEFPlotMainBase : public QMainWindow
{
    Q_OBJECT

public:
    CHEFPlotMainBase( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~CHEFPlotMainBase();

    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *Options;
    QPopupMenu *PopupMenuEditor_12;
    QPopupMenu *helpMenu;
    QToolBar *toolBar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* editUndoAction;
    QAction* editRedoAction;
    QAction* editCutAction;
    QAction* editCopyAction;
    QAction* editPasteAction;
    QAction* editFindAction;
    QAction* helpContentsAction;
    QAction* helpIndexAction;
    QAction* helpAboutAction;
    QAction* optionsGridAction;
    QAction* optionsThumbWheelsAction;
    QAction* optionsScalesAction;
    QAction* optionsLegendAction;
    QAction* optionsFontsAction;
    QAction* optionsLego_PlotAction;
    QAction* optionsZoomAction;
    QAction* optionsAxis_Focus_Left_AxisAction;
    QAction* optionsAxis_Focus_RightAction;
    QAction* optionsto_PyCHEFAction;
    QAction* optionsShow_DataAction;
    QAction* optionsInterpolationAction;
    QAction* optionsCoordinatesAction;
    QAction* Action;
    QAction* fileAttachAction;
    QAction* Action_2;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void exit();
    virtual void enableCoordinatesDisplay( bool );
    virtual void enableLegoPlot( bool );
    virtual void enableInterpolation( bool );
    virtual void enableLengend( bool set );
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void enableGrid( bool );
    virtual void enableThumbWheels( bool set );
    virtual void zoomUseLeftAxis();
    virtual void zoomUseRightAxis();
    virtual void showDataPoints( bool );

protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;
    QPixmap image4;
    QPixmap image5;
    QPixmap image6;
    QPixmap image7;
    QPixmap image8;
    QPixmap image9;

};

#endif // CHEFPLOTMAINBASE_H
