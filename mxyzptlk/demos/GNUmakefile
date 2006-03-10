
.SUFFIXES: 
.SUFFIXES: .o .cpp .cc 

#
FNALROOT = ../../RUNTIME

INCS = -I/usr/local/include/boost-1_33 -I$(FNALROOT)/include/gms -I$(FNALROOT)/include/mxyzptlk -I$(FNALROOT)/include/basic_toolkit 

LIBS = -L$(FNALROOT)/lib -lmxyzptlk -lbasic_toolkit -Wl,-rpath,$(FNALROOT)/lib 


all: Inverse_A_Test Inverse_B_Test Inverse_C_Test Inverse_D_Test Inverse_E_Test


concattest : concattest.o
	g++ -o concattest  concattest.o $(LIBS)

concattest.o: concattest.cc
	g++ $(INCS) -c concattest.cc

Inverse_E_Test : Inverse_E_Test.o
	g++ -o Inverse_E_Test  Inverse_E_Test.o $(LIBS)

Inverse_E_Test.o: Inverse_E_Test.cc
	g++ $(INCS) -c Inverse_E_Test.cc

Inverse_D_Test : Inverse_D_Test.o
	g++ -o Inverse_D_Test  Inverse_D_Test.o $(LIBS)

Inverse_D_Test.o: Inverse_D_Test.cc
	g++ $(INCS) -c Inverse_D_Test.cc

Inverse_C_Test : Inverse_C_Test.o
	g++ -o Inverse_C_Test  Inverse_C_Test.o $(LIBS)

Inverse_C_Test.o: Inverse_C_Test.cc
	g++ $(INCS) -c Inverse_C_Test.cc

Inverse_B_Test : Inverse_B_Test.o
	g++ -o Inverse_B_Test  Inverse_B_Test.o $(LIBS)

Inverse_B_Test.o: Inverse_B_Test.cc
	g++ $(INCS) -c Inverse_B_Test.cc

Inverse_A_Test : Inverse_A_Test.o
	g++ -o Inverse_A_Test  Inverse_A_Test.o $(LIBS)

Inverse_A_Test.o: Inverse_A_Test.cc
	g++ $(INCS) -c Inverse_A_Test.cc

clean:	
	\rm *.o; \rm Inverse_A_Test Inverse_B_Test Inverse_C_Test Inverse_D_Test Inverse_E_Test
