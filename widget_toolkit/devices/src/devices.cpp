#include <qapp.h>
#include "devicetablebase.h"
#include <qtable.h>

int main( int argc, char *argv[] )
{

    QApplication a( argc, argv );

    DeviceTableBase* wm  = new DeviceTableBase ( 0 );
 
    wm->resize( 500, 500 );

    a.setMainWidget(wm );
    
    wm->show();
    
    return a.exec();


}

   
