#!/bin/tcsh -f

( ./run_test test1 >! out1) >& err1
diff -w  out1 expout1
diff -w  err1 experr1

