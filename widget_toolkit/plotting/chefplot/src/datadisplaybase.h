/****************************************************************************
** Form interface generated from reading ui file 'datadisplaybase.ui'
**
** Created: Sun Jan 9 21:18:56 2005
**      by: The User Interface Compiler ($Id: datadisplaybase.h,v 1.1 2005/01/25 17:34:56 ostiguy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef DATADISPLAYBASE_H
#define DATADISPLAYBASE_H

#include <qvariant.h>
#include <qmainwindow.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QAction;
class QActionGroup;
class QToolBar;
class QPopupMenu;
class QTable;

class DataDisplayBase : public QMainWindow
{
    Q_OBJECT

public:
    DataDisplayBase( QWidget* parent = 0, const char* name = 0, WFlags fl = WType_TopLevel );
    ~DataDisplayBase();

    QTable* DataTable;
    QMenuBar *MenuBar;
    QPopupMenu *fileMenu;
    QPopupMenu *View;
    QToolBar *toolBar;
    QAction* fileNewAction;
    QAction* fileOpenAction;
    QAction* fileSaveAction;
    QAction* fileSaveAsAction;
    QAction* filePrintAction;
    QAction* fileExitAction;
    QAction* viewMergeAction;

public slots:
    virtual void fileNew();
    virtual void fileOpen();
    virtual void fileSave();
    virtual void fileSaveAs();
    virtual void filePrint();
    virtual void fileExit();
    virtual void viewMerge( bool );

protected:
    QHBoxLayout* DataDisplayBaseLayout;

protected slots:
    virtual void languageChange();

};

#endif // DATADISPLAYBASE_H
