.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I../include -I../../basic_toolkit/include

EXE = NewtonStep \
      concattest \
      coordTest \
      dfr \
      envTest \
      evaltest \
      g5 \
      hptest \
      orthogonalTest \
      pbtest \
      ranTest \
      survey \
      CMapTest \
      Inverse_B_Test \
      Inverse_C_Test \
      Inverse_D_Test \
      Inverse_E_Test \
      Lie_A_Test \
      Lie_B_Test \
      Lie_C_Test \
      Lie_D_Test \
      Lie_E_Test \
      Lie_F_Test \
      Lie_G_Test \
      Lie_H_Test \
      Lie_I_Test \
      Lie_J_Test \
      Limit_A_Test \
      Limit_B_Test \
      Limit_C_Test \
      Limit_D_Test \
      Limit_E_Test \
      Limit_F_Test \
      Limit_G_Test \
      Limit_H_Test \
      PlusEqualTest \
      SVDTest


SOURCES = $(EXE:.o=.cc)
LIBS = -lmxyzptlk -lbasic_toolkit -lm


.cc.o:
	$(C++) -c $(C++FLAGS) $(INCLUDEDIR) $*.cc
.c.o:
	$(C) -c $(CFLAGS) $(INCLUDEDIR) $*.c
.cc :
	$(C++) $(C++FLAGS) $(INCLUDEDIR) -o $* $*.cc \
        -L$(LIBDIR) $(LIBS)
.o :
	$(C++) $(C++FLAGS) -o $@ \
        $< -L$(LIBDIR) $(LIBS)



include ../../Make-config


targets: $(EXE)


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies
