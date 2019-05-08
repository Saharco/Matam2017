#!/bin/tcsh -f

( ./run_test test10 >! out10) >& err10
diff -w  out10 expout10
diff -w  err10 experr10

