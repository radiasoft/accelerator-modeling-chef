#ifndef APPWORKSPACE_H
#define APPWORKSPACE_H

#include <qworkspace.h>


class AppWorkspace:public QWorkspace {

Q_OBJECT
    
public:
        AppWorkspace(QWidget *parent = 0, const char *name = 0)
            : QWorkspace(parent, name)
	{}
 virtual ~AppWorkspace() 
        {} 

    
public slots:

        void tileVertically();
        void tileHorizontally();
        void cascade();        
        void defaultLayout();
};
    
#endif
