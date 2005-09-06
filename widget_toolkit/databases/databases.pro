########################################################
# databases.pro
#
# Author: Jean-Francois Ostiguy
# ostiguy@fnal.gov
#
#########################################################

TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_on release

include ( ../../chef-config/config.pri )

HEADERS	+= include/DbConnectDialog.h \
	   include/DbRetrieveDialog.h \
           include/DbRetrieveQueries.h

SOURCES	+= src/DbConnectDialog.cpp \
	   src/DbRetrieveDialog.cpp

FORMS	= src/dbconnectdialogbase.ui \
	  src/dbretrievedialogbase.ui

