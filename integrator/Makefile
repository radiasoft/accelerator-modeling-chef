.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(INCDIR)


INCS = include/*.h
#INCS += include/*.rsc
#INCS += include/*.inc


include $(FNALROOT)/Make-config

all: link

link:
	@set -x; for i in $(INCS); do  \
	 (ln -s $(FNALROOT)/integrator/$$i $(FNALROOT)/$$i ); \
		done

clean:
	rm -f *.o *~*

