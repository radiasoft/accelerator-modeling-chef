C++ = Bcc32
ILINK32 = Ilink32
TLINK32 = Tlink32


IncDirectories = -IC:\BC5\INCLUDE;..\INCLUDE;..\..\MXB\INCLUDE;
DefinedSymbols = -D_RTLDLL;_BIDSDLL;__BORLAND_CPP__;
LibDirectories = -LC:\BC5\LIB;C:\winnt\profiles\michelot\projects\bmlmxb\mxb\lib;C:\winnt\profiles\michelot\projects\bmlmxb\bml\lib;
Libraries = phys.lib bml.lib mxb.lib cxxlibr.lib bidsfi.lib import32.lib cw32i.lib


demoFiles = \
FODO_E.exe \
boosterv6.exe \
norm_A.exe \
norm_B.exe \
normalFormTest.exe \
sbendTest.exe


all: $(demoFiles)


.obj.exe:
  $(ILINK32) @&&| 
  $(LibDirectories) -x \
  C:\BC5\LIB\c0x32.obj \
  $&.obj,  $@, , \
  $(Libraries), , 
|

.cc.obj:
    $(C++) -c -P @&&|
    -WC $(IncDirectories) $(DefinedSymbols) -o$@ $&.cc
|

# Compiler configuration file
BccW32.cfg : 
   Copy &&|
-w
-R
-v
-WM-
-vi
-H
-H=mxyzptlk.csm
| $@
