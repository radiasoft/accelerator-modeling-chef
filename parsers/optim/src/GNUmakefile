.SUFFIXES:
.SUFFIXES: .l .cc .c .o 


VPATH=../include

FNALROOT = /home/ostiguy/CHEF/fnal-apr-20/fnal/RUNTIME
INCS = -I. -I../include -I$(FNALROOT)/include/basic_toolkit -I$(FNALROOT)/include/beamline -I$(FNALROOT)/include/mxyzptlk  -I$(FNALROOT)/include/gms 

CXXFLAGS = -g 
#CXXFLAGS = -O2

parse_optim: parse_optim.o lex.optim_yy.c OptimParser_ypp.o OptimParserDriver.o
	g++  $(CXXFLAGS) $(INCS) -o parse_optim  parse_optim.o  lex.optim_yy.c OptimParser_ypp.o OptimParserDriver.o

lex.optim_yy.c: optim.l
	flex -d optim.l

OptimParser_ypp.cc: optim.ypp
	bison -v optim.ypp

parse_optim.o: parse_optim.cc
	g++ $(INCS) $(CXXFLAGS) -c  parse_optim.cc

OptimParser_ypp.o: OptimParser_ypp.cc OptimParser_ypp.hh
	g++ $(INCS) $(CXXFLAGS) -c OptimParser_ypp.cc

OptimParserDriver.o: OptimParserDriver.cc OptimParserDriver.h
	g++ $(INCS) $(CXXFLAGS) -c  OptimParserDriver.cc


clean: 
	\rm OptimParser_ypp.hh OptimParser_ypp.cc location.hh stack.hh position.hh lex.optim_yy.c *.o parse_optim *.output

