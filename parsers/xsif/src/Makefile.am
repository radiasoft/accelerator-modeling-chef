lib_LTLIBRARIES                   =    libxsifparser.la  

libxsifparser_la_SOURCES          =    XsifParser_ypp.cc lex.xsif_yy.cc Expression.cc XSIFFactory.cc XsifParserDriver.cc 

AM_CPPFLAGS                       =   -I$(BOOST_INC) -I$(VSQLITEXX_INC) -I. -I$(top_srcdir)/include  -I$(top_srcdir)/include/tree-container -I$(top_srcdir)/../../include -I$(GLIB_INC) -I$(GLIBCONFIG_INC) 

LEX  = flex
YACC = bison 

XsifParser_ypp.hh: XsifParser_ypp.cc

lex.xsif_yy.cc:  xsif_lex.l
	flex -o lex.xsif_yy.cc -d $^ 

XsifParser_ypp.cc: xsif_yacc.ypp
	bison -v  $^ 




