#!/bin/tcsh -f

( ./run_test test15 >! out15) >& err15
diff -w  out15 expout15
diff -w  err15 experr15

