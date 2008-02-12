#!/bin/csh

./SVDFitTest 2 2 >  SVDFitTest.out
./SVDFitTest 3 2 >> SVDFitTest.out
./SVDFitTest 4 2 >> SVDFitTest.out
./SVDFitTest 5 2 >> SVDFitTest.out
./SVDFitTest 6 2 >> SVDFitTest.out
./SVDFitTest 7 2 >> SVDFitTest.out


./SVDFitTest 3 3 >> SVDFitTest.out
./SVDFitTest 4 3 >> SVDFitTest.out
./SVDFitTest 5 3 >> SVDFitTest.out
./SVDFitTest 6 3 >> SVDFitTest.out
./SVDFitTest 7 3 >> SVDFitTest.out

## rm SVDFitTest.out
