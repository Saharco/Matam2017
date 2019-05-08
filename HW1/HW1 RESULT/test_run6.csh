#!/bin/tcsh -f

( ./run_test test6 >! out6) >& err6
diff -w  out6 expout6
diff -w  err6 experr6

