#!/bin/csh
   
./Lie_A_Test  1.1  1.1  >& Lie_A_Test.out
./Lie_A_Test -1.1  1.1 >>& Lie_A_Test.out
./Lie_A_Test  1.1 -1.1 >>& Lie_A_Test.out
./Lie_A_Test -1.1 -1.1 >>& Lie_A_Test.out
