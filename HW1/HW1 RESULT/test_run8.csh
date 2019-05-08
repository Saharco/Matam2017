#!/bin/tcsh -f

( ./run_test test8 >! out8) >& err8
diff -w  out8 expout8
diff -w  err8 experr8

