#!/bin/tcsh -f

( ./run_test test22 >! out22) >& err22
diff -w  out22 expout22
diff -w  err22 experr22

