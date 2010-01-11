#!/bin/csh
   
./Lie_G_Test 5 0.1  >& Lie_G_Test.out
./Lie_G_Test 5 0.2 >>& Lie_G_Test.out
./Lie_G_Test 5 0.5 >>& Lie_G_Test.out
./Lie_G_Test 5 1.0 >>& Lie_G_Test.out
