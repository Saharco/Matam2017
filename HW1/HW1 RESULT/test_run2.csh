#!/bin/tcsh -f

( ./run_test test2 >! out2) >& err2
diff -w  out2 expout2
diff -w  err2 experr2

