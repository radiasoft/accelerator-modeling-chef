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


#-----------------------------------------------------------------------------------

token-test: token-test.cc
	g++  $(CXXFLAGS) $(INCS) -o token-test token-test.cc 

expression_tree: expression_tree.cc
	g++  $(CXXFLAGS) $(INCS) -o expression_tree expression_tree.cc 

ExpressionTree.o: ExpressionTree.cc
	g++ $(INCS) $(CXXFLAGS) -c ExpressionTree.cc 

tree-bug-2: tree-bug-2.cc
	g++  $(CXXFLAGS) $(INCS) -o tree-bug-2 tree-bug-2.cc 

tree-bug: tree-bug.cc
	g++  $(CXXFLAGS) $(INCS) -o tree-bug tree-bug.cc 

clean: 
	\rm MadxParser_ypp.hh MadxParser_ypp.cc location.hh stack.hh position.hh lex.madx_yy.c *.o parse_madx *.output

