C++ = Bcc32
ILINK32 = Ilink32
TLINK32 = Tlink32

IncDirectories = -IC:\BC5\INCLUDE;..\INCLUDE;..\..\BML\INCLUDE;
DefinedSymbols = -D_RTLDLL;_BIDSDLL;__BORLAND_CPP__;
LibDirectories = -LC:\BC5\LIB;C:\winnt\profiles\michelot\projects\bmlmxb\mxb\lib
Libraries = mxb.LIB cxxlibr.LIB bidsfi.lib import32.lib cw32i.lib

demoFiles = \
CMapTest.exe \
Inverse_B_Test.exe \
Inverse_C_Test.exe \
Inverse_D_Test.exe \
Inverse_E_Test.exe \
Lie_A_Test.exe \
Lie_B_Test.exe \
Lie_C_Test.exe \
Lie_D_Test.exe \
Lie_E_Test.exe \
Lie_F_Test.exe \
Lie_G_Test.exe \
Lie_H_Test.exe \
Lie_I_Test.exe \
Lie_J_Test.exe \
Limit_A_Test.exe \
Limit_B_Test.exe \
Limit_C_Test.exe \
Limit_D_Test.exe \
Limit_E_Test.exe \
Limit_F_Test.exe \
Limit_G_Test.exe \
Limit_H_Test.exe \
SVDTest.exe \
concattest.exe \
coordTest.exe \
dfr.exe \
envTest.exe \
evaltest.exe \
g5.exe \
hptest.exe \
orthogonalTest.exe \
pbtest.exe \
survey.exe 


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


