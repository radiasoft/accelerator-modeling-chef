/****************************************************************************
** Form interface generated from reading ui file 'plotpropertiesdialog.ui'
**
** Created: Wed Jul 7 16:19:24 2004
**      by: The User Interface Compiler ($Id: plotpropertiesdialog.h,v 1.2 2004/07/16 21:46:14 michelot Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef PLOTPROPERTIESDIALOG_H
#define PLOTPROPERTIESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include "plotproperties.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QTabWidget;
class QWidget;
class QCheckBox;
class QLabel;
class QComboBox;
class QLineEdit;
class QListBox;
class QListBoxItem;

class PlotPropertiesDialog : public QDialog
{
    Q_OBJECT

public:
    PlotPropertiesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~PlotPropertiesDialog();

    QPushButton* buttonCancel;
    QPushButton* buttonOk;
    QPushButton* apply_pushButton;
    QPushButton* buttonHelp;
    QTabWidget* tabWidget;
    QWidget* Widget8;
    QCheckBox* checkBox3_2;
    QLabel* textLabel1_2;
    QComboBox* comboBox1;
    QLabel* textLabel2_2;
    QComboBox* comboBox2;
    QComboBox* comboBox3;
    QLabel* textLabel1_3;
    QCheckBox* checkBox_gridLines;
    QWidget* TabPage;
    QLabel* textLabel1;
    QLineEdit* lineEdit1;
    QLineEdit* lineEdit3;
    QLabel* textLabel2;
    QLabel* textLabel5;
    QLabel* textLabel6;
    QLineEdit* lineEdit5;
    QLabel* textLabel3;
    QLabel* textLabel4;
    QLineEdit* lineEdit6;
    QLabel* textLabel7;
    QLabel* textLabel10;
    QLabel* textLabel5_2;
    QLabel* textLabel11;
    QLabel* textLabel9;
    QLineEdit* lineEdit8;
    QLabel* textLabel8;
    QLineEdit* lineEdit7;
    QLineEdit* lineEdit4;
    QLineEdit* lineEdit2;
    QCheckBox* checkBox3;
    QCheckBox* checkBox4;
    QCheckBox* checkBox6;
    QCheckBox* checkBox5;
    QWidget* Widget9;
    QListBox* listBox1;
    QLabel* textLabel12;
    QWidget* TabPage_2;

public slots:
    virtual void checkBox_gridLines_stateChanged( int state );
    virtual void plotPropertiesApply();

signals:
    void plot_properties_changed(const PlotProperties& properties);

protected:

protected slots:
    virtual void languageChange();

private:
    PlotProperties _properties;

};

#endif // PLOTPROPERTIESDIALOG_H
