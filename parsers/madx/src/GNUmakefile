.SUFFIXES:
.SUFFIXES: .l .cc .c .o 


VPATH=../include

FNALROOT = /home/ostiguy/CHEF/fnal-apr-20/fnal/RUNTIME
INCS = -I. -I../include -I$(FNALROOT)/include -I../include/tree-container 

CXXFLAGS = -g -O0 
#CXXFLAGS = -O2

parse_madx: parse_madx.o lex.madx_yy.c MadxParser_ypp.o MadxParserDriver.o Expression.o
	g++  $(CXXFLAGS) $(INCS) -o parse_madx  parse_madx.o  lex.madx_yy.c MadxParser_ypp.o MadxParserDriver.o Expression.o

lex.madx_yy.c: madx.l
	flex -d madx.l

MadxParser_ypp.cc: madx.ypp
	bison -v madx.ypp

parse_madx.o: parse_madx.cc
	g++ $(INCS) $(CXXFLAGS) -c  parse_madx.cc

MadxParser_ypp.o: MadxParser_ypp.cc MadxParser_ypp.hh
	g++ $(INCS) $(CXXFLAGS) -c MadxParser_ypp.cc

MadxParserDriver.o: MadxParserDriver.cc MadxParserDriver.h
	g++ $(INCS) $(CXXFLAGS) -c  MadxParserDriver.cc

Expression.o: Expression.cc Expression.h
	g++ $(INCS) $(CXXFLAGS) -c  Expression.cc


clean: 
	\rm MadxParser_ypp.hh MadxParser_ypp.cc location.hh stack.hh position.hh lex.madx_yy.c *.o parse_madx *.output

