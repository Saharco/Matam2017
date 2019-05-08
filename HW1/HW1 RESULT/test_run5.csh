#!/bin/tcsh -f

( ./run_test test5 >! out5) >& err5
diff -w  out5 expout5
diff -w  err5 experr5

