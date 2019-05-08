#!/bin/tcsh -f

( ./run_test test3 >! out3) >& err3
diff -w  out3 expout3
diff -w  err3 experr3

