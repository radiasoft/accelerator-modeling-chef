/****************************************************************************
** Form implementation generated from reading ui file 'devicetablebase.ui'
**
** Created: Fri Jan 7 16:06:25 2005
**      by: The User Interface Compiler ($Id: devicetablebase.cpp,v 1.1 2005/01/25 17:24:21 ostiguy Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "devicetablebase.h"

#include <qvariant.h>
#include <qtable.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "devicetablebase.ui.h"

/*
 *  Constructs a DeviceTableBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
DeviceTableBase::DeviceTableBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "DeviceTableBase" );
    DeviceTableBaseLayout = new QHBoxLayout( this, 11, 6, "DeviceTableBaseLayout"); 

    device_table = new QTable( this, "device_table" );
    device_table->setNumRows( 3 );
    device_table->setNumCols( 3 );
    DeviceTableBaseLayout->addWidget( device_table );
    languageChange();
    resize( QSize(542, 423).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
DeviceTableBase::~DeviceTableBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void DeviceTableBase::languageChange()
{
    setCaption( tr( "ACNET Devices Monitor" ) );
}

