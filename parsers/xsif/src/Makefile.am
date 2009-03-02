lib_LTLIBRARIES                   =    libxsifparser.la  

libxsifparser_la_SOURCES          =    XsifParser_ypp.cc lex.xsif_yy.cc Expression.cc XSIFFactory.cc XsifParserDriver.cc 

AM_CPPFLAGS                       =    $(BOOST_INC) $(FFTW3_INC) $(VSQLITEXX_INC) -I. -I$(top_srcdir)/include  -I$(top_srcdir)/include/tree-container -I$(top_srcdir)/../../include  

libxsifparser_la_LDFLAGS          =  -lboost_filesystem-mt  -lboost_system-mt

LEX  = flex
YACC = bison 

XsifParser_ypp.hh: XsifParser_ypp.cc

lex.xsif_yy.cc:  xsif_lex.l
	flex -o lex.xsif_yy.cc -d $^ 

XsifParser_ypp.cc: xsif_yacc.ypp
	bison -v  $^ 




