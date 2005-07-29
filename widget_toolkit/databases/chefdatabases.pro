TEMPLATE	= lib
LANGUAGE	= C++

CONFIG	+= qt warn_on release

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}

HEADERS	+= DbConnectDialog.h \
	DbRetrieveDialog.h \
	DbRetrieveQueries.h

SOURCES	+= DbConnectDialog.cpp \
	DbRetrieveDialog.cpp

FORMS	= dbconnectdialogbase.ui \
	dbretrievedialogbase.ui

