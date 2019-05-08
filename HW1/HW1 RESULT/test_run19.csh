#!/bin/tcsh -f

( ./run_test test19 >! out19) >& err19
diff -w  out19 expout19
diff -w  err19 experr19

