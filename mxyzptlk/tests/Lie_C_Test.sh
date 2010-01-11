#!/bin/csh
   
./Lie_C_Test 3 0.1  >& Lie_C_Test.out
./Lie_C_Test 3 0.2 >>& Lie_C_Test.out
./Lie_C_Test 3 0.5 >>& Lie_C_Test.out
./Lie_C_Test 3 1.0 >>& Lie_C_Test.out
./Lie_C_Test 5 1.0 >>& Lie_C_Test.out
./Lie_C_Test 7 1.0 >>& Lie_C_Test.out
